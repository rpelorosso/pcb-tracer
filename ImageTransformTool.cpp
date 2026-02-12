#include "ImageTransformTool.h"
#include "Editor.h"
#include "ImageLayer.h"
#include "actions/AdjustPerspective.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

static const qreal HANDLE_RADIUS = 8.0;
static const qreal EDGE_HANDLE_RADIUS = 6.0;

TransformHandle::TransformHandle(int index, HandleCallbacks callbacks)
    : QGraphicsEllipseItem()
    , m_index(index)
    , m_callbacks(std::move(callbacks))
{
    qreal r = (index < 4) ? HANDLE_RADIUS : EDGE_HANDLE_RADIUS;
    setRect(-r, -r, r * 2, r * 2);
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIgnoresTransformations);
    if (index < 4) {
        setPen(QPen(Qt::yellow));
        setBrush(QColor(255, 255, 0, 160));
    } else {
        setPen(QPen(Qt::cyan));
        setBrush(QColor(0, 255, 255, 160));
    }
    setZValue(100);
}

QVariant TransformHandle::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionChange && m_dragging && m_index >= 4) {
        QPointF newPos = value.toPointF();
        if (m_index == 4 || m_index == 6)
            newPos.setX(pos().x());
        else
            newPos.setY(pos().y());
        return newPos;
    }
    if (change == ItemPositionHasChanged && m_callbacks.onMoved)
        m_callbacks.onMoved(m_index);
    return QGraphicsEllipseItem::itemChange(change, value);
}

void TransformHandle::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    m_dragging = true;
    if (m_callbacks.onDragStarted)
        m_callbacks.onDragStarted(m_index);
    QGraphicsEllipseItem::mousePressEvent(event);
}

void TransformHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    m_dragging = false;
    if (m_callbacks.onDragFinished)
        m_callbacks.onDragFinished(m_index);
}

ImageTransformTool::ImageTransformTool(Editor* editor, ImageLayer* layer)
    : m_layer(layer)
{
    m_editor = editor;
}

ImageTransformTool::~ImageTransformTool() {
    removeHandles();
}

void ImageTransformTool::enterMode() {
    createHandles();
    m_editor->getScene()->setSceneRect(m_editor->getScene()->itemsBoundingRect());
    m_editor->fitInView(m_layer, Qt::KeepAspectRatio);
    m_editor->showStatusMessage("Perspective transform: drag corners. Press Escape to finish.");
    m_undoConnection = QObject::connect(
        &m_editor->m_undoStack, &QUndoStack::indexChanged,
        m_editor, [this]() {
            m_updatingHandles = true;
            updateHandlePositions();
            m_updatingHandles = false;
        });
}

void ImageTransformTool::exitMode() {
    QObject::disconnect(m_undoConnection);
    m_editor->showStatusMessage("");
    removeHandles();
    m_editor->getScene()->setSceneRect(m_editor->getScene()->itemsBoundingRect());
    m_editor->centerOn(m_layer);
}

static QPointF sceneCorner(ImageLayer* layer, int cornerIdx) {
    QRectF r = layer->pixmap().rect().toRectF();
    QPointF src[4] = {r.topLeft(), r.topRight(), r.bottomRight(), r.bottomLeft()};
    return layer->mapToScene(src[cornerIdx]);
}

void ImageTransformTool::createHandles() {
    removeHandles();
    QGraphicsScene* scene = m_editor->getScene();

    HandleCallbacks callbacks{
        [this](int idx) { handleMoved(idx); },
        [this](int idx) { handleDragStarted(idx); },
        [this](int idx) { handleDragFinished(idx); }
    };

    for (int i = 0; i < 4; ++i) {
        m_handles[i] = new TransformHandle(i, callbacks);
        m_handles[i]->setPos(sceneCorner(m_layer, i));
        scene->addItem(m_handles[i]);
    }

    for (int i = 0; i < 4; ++i) {
        int a = EDGE_PAIRS[i][0], b = EDGE_PAIRS[i][1];
        QPointF mid = (sceneCorner(m_layer, a) + sceneCorner(m_layer, b)) / 2.0;
        m_handles[4 + i] = new TransformHandle(4 + i, callbacks);
        m_handles[4 + i]->setPos(mid);
        scene->addItem(m_handles[4 + i]);
    }
}

void ImageTransformTool::removeHandles() {
    for (int i = 0; i < HANDLE_COUNT; ++i) {
        if (m_handles[i]) {
            if (m_handles[i]->scene())
                m_handles[i]->scene()->removeItem(m_handles[i]);
            delete m_handles[i];
            m_handles[i] = nullptr;
        }
    }
}

void ImageTransformTool::updateHandlePositions() {
    for (int i = 0; i < 4; ++i) {
        if (m_handles[i])
            m_handles[i]->setPos(sceneCorner(m_layer, i));
    }
    for (int i = 0; i < 4; ++i) {
        if (m_handles[4 + i]) {
            int a = EDGE_PAIRS[i][0], b = EDGE_PAIRS[i][1];
            m_handles[4 + i]->setPos((sceneCorner(m_layer, a) + sceneCorner(m_layer, b)) / 2.0);
        }
    }
}

void ImageTransformTool::handleMoved(int index) {
    if (m_updatingHandles) return;

    QPointF scenePos = m_handles[index]->pos();

    QTransform t = m_layer->transform();
    m_layer->setTransform(QTransform());

    if (index < 4) {
        QPointF localPos = m_layer->mapFromScene(scenePos);
        m_layer->m_corners[index] = localPos;
    } else {
        int edgeIdx = index - 4;
        int a = EDGE_PAIRS[edgeIdx][0], b = EDGE_PAIRS[edgeIdx][1];
        QPointF oldMidLocal = (m_layer->m_corners[a] + m_layer->m_corners[b]) / 2.0;
        QPointF newMidLocal = m_layer->mapFromScene(scenePos);
        QPointF delta = newMidLocal - oldMidLocal;
        m_layer->m_corners[a] += delta;
        m_layer->m_corners[b] += delta;
    }

    m_layer->setTransform(t);
    m_layer->applyPerspectiveTransform();

    m_updatingHandles = true;
    updateHandlePositions();
    m_updatingHandles = false;
}

void ImageTransformTool::handleDragStarted(int index) {
    m_cornersBeforeDrag = m_layer->m_corners;
}

void ImageTransformTool::handleDragFinished(int index) {
    if (m_cornersBeforeDrag == m_layer->m_corners) return;

    AdjustPerspectiveMeta meta{m_layer->m_id, m_cornersBeforeDrag, m_layer->m_corners};
    auto* action = new AdjustPerspective(meta);
    m_editor->m_undoStack.push(action);
}

bool ImageTransformTool::onKeyPress(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        m_editor->enterTrackMode();
        return true;
    }
    return false;
}

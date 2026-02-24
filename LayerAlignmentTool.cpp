#include "LayerAlignmentTool.h"
#include "Editor.h"
#include "ImageLayer.h"
#include "enums.h"
#include "actions/AlignLayer.h"
#include <QGraphicsScene>
#include <QPolygonF>
#include <QTransform>
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <cmath>

LayerAlignmentTool::LayerAlignmentTool(Editor* editor)
{
    m_editor = editor;
    m_phase = Phase::PICK_FRONT;
}

void LayerAlignmentTool::enterMode()
{
    m_editor->setCursor(Qt::CrossCursor);
    m_phase = Phase::PICK_FRONT;
    m_frontPoints.clear();
    m_backPoints.clear();
    clearMarkers();

    // Verify both layers exist
    int frontId = static_cast<int>(LinkSide::FRONT);
    int backId = static_cast<int>(LinkSide::BACK);
    ImageLayer* frontLayer = Editor::instance()->findItemByIdAndClass<ImageLayer>(frontId);
    ImageLayer* backLayer = Editor::instance()->findItemByIdAndClass<ImageLayer>(backId);

    if (!frontLayer || !backLayer) {
        m_editor->showStatusMessage("Both front and back images must be loaded first.");
        m_editor->enterTrackMode();
        return;
    }

    QMessageBox::information(m_editor, "Align Layers",
        "Pick 4 matching points on each layer to compute an alignment transform.\n\n"
        "1. Click 4 points on the FRONT layer\n"
        "2. Click the same 4 points on the BACK layer\n"
        "3. Choose which layer to transform\n\n"
        "Tip: For best results, pick points near each corner of the board.\n\n"
        "Press Escape to cancel at any time.");

    m_editor->showStatusMessage("Click 4 points on the FRONT layer (1/4)");
}

void LayerAlignmentTool::exitMode()
{
    clearMarkers();
    m_editor->setCursor(Qt::ArrowCursor);
}

bool LayerAlignmentTool::onMousePress(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return false;

    QPointF scenePos = m_editor->mapToScene(event->pos());

    if (m_phase == Phase::PICK_FRONT) {
        m_frontPoints.push_back(scenePos);
        addMarker(scenePos, m_frontPoints.size(), QColor(0, 200, 0));

        if (m_frontPoints.size() < 4) {
            m_editor->showStatusMessage(
                QString("Click 4 points on the FRONT layer (%1/4)").arg(m_frontPoints.size() + 1));
        } else {
            m_phase = Phase::PICK_BACK;
            m_editor->showStatusMessage("Click 4 corresponding points on the BACK layer (1/4)");
        }
    } else if (m_phase == Phase::PICK_BACK) {
        m_backPoints.push_back(scenePos);
        addMarker(scenePos, m_backPoints.size(), QColor(60, 120, 255));

        if (m_backPoints.size() < 4) {
            m_editor->showStatusMessage(
                QString("Click 4 corresponding points on the BACK layer (%1/4)").arg(m_backPoints.size() + 1));
        } else {
            applyAlignment();
        }
    }

    return true;
}

bool LayerAlignmentTool::onKeyPress(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        cancel();
        return true;
    }
    return false;
}

void LayerAlignmentTool::addMarker(const QPointF& scenePos, int index, const QColor& color)
{
    const qreal radius = 8;
    auto* circle = new QGraphicsEllipseItem(
        -radius, -radius, radius * 2, radius * 2);
    circle->setPos(scenePos);
    circle->setPen(QPen(color, 2));
    circle->setBrush(QBrush(QColor(color.red(), color.green(), color.blue(), 80)));
    circle->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    circle->setZValue(100);
    m_editor->scene()->addItem(circle);

    auto* label = new QGraphicsSimpleTextItem(QString::number(index));
    label->setPos(scenePos.x() + radius, scenePos.y() - radius);
    label->setBrush(color);
    QFont font;
    font.setBold(true);
    font.setPointSize(12);
    label->setFont(font);
    label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    label->setZValue(100);
    m_editor->scene()->addItem(label);

    m_markers.push_back({circle, label});
}

bool LayerAlignmentTool::computeAffineTransform(
    const QPolygonF& srcLocal, const QPolygonF& dstLocal, QTransform& result)
{
    int n = 4;
    double sumX = 0, sumY = 0, sumXX = 0, sumXY = 0, sumYY = 0;
    double sumXpX = 0, sumXpY = 0, sumYpX = 0, sumYpY = 0;
    double sumXp = 0, sumYp = 0;

    for (int i = 0; i < n; ++i) {
        double x = srcLocal[i].x(), y = srcLocal[i].y();
        double xp = dstLocal[i].x(), yp = dstLocal[i].y();
        sumX += x;    sumY += y;
        sumXX += x*x; sumXY += x*y; sumYY += y*y;
        sumXpX += xp*x; sumXpY += xp*y;
        sumYpX += yp*x; sumYpY += yp*y;
        sumXp += xp;  sumYp += yp;
    }

    double A[3][3] = {
        {sumXX, sumXY, sumX},
        {sumXY, sumYY, sumY},
        {sumX,  sumY,  (double)n}
    };

    auto det3 = [](double m[3][3]) {
        return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
             - m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
             + m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
    };

    double detA = det3(A);
    if (std::abs(detA) < 1e-10)
        return false;

    double rhsX[3] = {sumXpX, sumXpY, sumXp};
    double Ax[3][3], Ay[3][3], Az[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            Ax[i][j] = A[i][j]; Ay[i][j] = A[i][j]; Az[i][j] = A[i][j];
        }
    for (int i = 0; i < 3; ++i) { Ax[i][0] = rhsX[i]; Ay[i][1] = rhsX[i]; Az[i][2] = rhsX[i]; }
    double a  = det3(Ax) / detA;
    double b  = det3(Ay) / detA;
    double tx = det3(Az) / detA;

    double rhsY[3] = {sumYpX, sumYpY, sumYp};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            Ax[i][j] = A[i][j]; Ay[i][j] = A[i][j]; Az[i][j] = A[i][j];
        }
    for (int i = 0; i < 3; ++i) { Ax[i][0] = rhsY[i]; Ay[i][1] = rhsY[i]; Az[i][2] = rhsY[i]; }
    double c  = det3(Ax) / detA;
    double d  = det3(Ay) / detA;
    double ty = det3(Az) / detA;

    // QTransform(m11, m12, m21, m22, dx, dy) where:
    //   x' = m11*x + m21*y + dx
    //   y' = m12*x + m22*y + dy
    result = QTransform(a, c, b, d, tx, ty);
    return true;
}

void LayerAlignmentTool::applyAlignment()
{
    // Ask which layer to transform
    QMessageBox msgBox(m_editor);
    msgBox.setWindowTitle("Align Layers");
    msgBox.setText("Which layer should be transformed?");
    QPushButton* backBtn = msgBox.addButton("Back", QMessageBox::AcceptRole);
    QPushButton* frontBtn = msgBox.addButton("Front", QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.exec();

    QAbstractButton* clicked = msgBox.clickedButton();
    if (clicked != static_cast<QAbstractButton*>(backBtn) &&
        clicked != static_cast<QAbstractButton*>(frontBtn)) {
        cancel();
        return;
    }

    bool transformBack = (clicked == static_cast<QAbstractButton*>(backBtn));
    LinkSide targetSide = transformBack ? LinkSide::BACK : LinkSide::FRONT;
    int targetId = static_cast<int>(targetSide);
    ImageLayer* targetLayer = Editor::instance()->findItemByIdAndClass<ImageLayer>(targetId);
    if (!targetLayer) {
        cancel();
        return;
    }

    // Source points = points on the layer being moved
    // Destination points = points on the layer staying fixed
    // If transforming back: back points move to where front points are
    // If transforming front: front points move to where back points are
    const auto& srcScenePoints = transformBack ? m_backPoints : m_frontPoints;
    const auto& dstScenePoints = transformBack ? m_frontPoints : m_backPoints;

    QPolygonF srcLocal, dstLocal;
    for (int i = 0; i < 4; ++i) {
        srcLocal << targetLayer->mapFromScene(srcScenePoints[i]);
        dstLocal << targetLayer->mapFromScene(dstScenePoints[i]);
    }

    QTransform transform;
    if (!computeAffineTransform(srcLocal, dstLocal, transform)) {
        m_editor->showStatusMessage("Failed to compute alignment (degenerate points).");
        cancel();
        return;
    }

    QTransform oldTransform = targetLayer->transform();
    QTransform newTransform = transform * oldTransform;

    AlignLayerMeta meta;
    meta.layerId = targetId;
    meta.oldTransform = oldTransform;
    meta.newTransform = newTransform;

    m_editor->m_undoStack.push(new AlignLayer(meta));

    clearMarkers();
    m_editor->showStatusMessage("Layer alignment applied.");
    m_editor->enterTrackMode();
}

void LayerAlignmentTool::cancel()
{
    clearMarkers();
    m_frontPoints.clear();
    m_backPoints.clear();
    m_editor->showStatusMessage("");
    m_editor->enterTrackMode();
}

void LayerAlignmentTool::clearMarkers()
{
    for (auto& marker : m_markers) {
        m_editor->scene()->removeItem(marker.circle);
        m_editor->scene()->removeItem(marker.label);
        delete marker.circle;
        delete marker.label;
    }
    m_markers.clear();
}

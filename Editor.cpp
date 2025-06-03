#include <QDebug>
#include "Editor.h"
#include "ConnectionAnalyzer.h"
#include "TrackDrawingTool.h"
#include "ComponentDrawingTool.h"
#include "ZoomableGraphicsView.h"
#include <QGraphicsLineItem>
#include "QGraphicsItemLayer.h"
#include "NotesTool.h"

Editor* Editor::m_instance = nullptr;

Editor* Editor::instance(QWidget* parent) {
    if (m_instance == nullptr) {
        m_instance = new Editor(parent);
    }
    return m_instance;
}
Editor::Editor(QWidget* parent) : ZoomableGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
	m_scene->setBackgroundBrush(QBrush(QColor(28, 30, 32)));

    setMouseTracking(true);

    m_tracingIndicator = addTracingIndicator();
    m_trackDrawingTool = new TrackDrawingTool(this);
	m_componentDrawingTool = new ComponentDrawingTool();
	m_notesTool = new NotesTool(this);
    m_currentTool = m_trackDrawingTool;
	m_currentSide = LinkSide::FRONT;
	m_state = DrawingState::TRACKS;
	m_guideTool = new GuideTool();
    enterTrackMode();

	m_layers = {
        {LinkSide::BACK, new QGraphicsItemLayer()},
        {LinkSide::FRONT, new QGraphicsItemLayer()},
        {LinkSide::NODE, new QGraphicsItemLayer()},
        {LinkSide::WIP, new QGraphicsItemLayer()},
        {LinkSide::NOTES, new QGraphicsItemLayer()}
    };

    m_scene->addItem(m_layers[LinkSide::FRONT]);
    m_scene->addItem(m_layers[LinkSide::BACK]);
    m_scene->addItem(m_layers[LinkSide::WIP]);
    m_scene->addItem(m_layers[LinkSide::NODE]);
    m_scene->addItem(m_layers[LinkSide::NOTES]);

    m_layers[LinkSide::BACK]->setZValue(0);
    m_layers[LinkSide::FRONT]->setZValue(1);
    m_layers[LinkSide::WIP]->setZValue(2);
    m_layers[LinkSide::NODE]->setZValue(3);
    m_layers[LinkSide::NOTES]->setZValue(5);

}

Editor::~Editor() {
    delete m_guideTool;
	delete m_trackDrawingTool;
	delete m_tracingIndicator;
	delete m_componentDrawingTool;
	delete m_notesTool;
}


void Editor::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Z && event->modifiers() & Qt::ControlModifier) {
        if (event->modifiers() & Qt::ShiftModifier) {
            m_undoStack.redo();
        } else {
            m_undoStack.undo();
        }
    } else if (!m_currentTool->onKeyPress(event)) {
        ZoomableGraphicsView::keyPressEvent(event);
    }
}

void Editor::keyReleaseEvent(QKeyEvent* event) {
    if (!m_currentTool->onKeyRelease(event)) {
		ZoomableGraphicsView::keyReleaseEvent(event);
    }}

void Editor::mouseDoubleClickEvent(QMouseEvent* event) {
    if (!m_currentTool->onMouseDoubleClick(event)) {
		ZoomableGraphicsView::mouseDoubleClickEvent(event);
    }
}

void Editor::mousePressEvent(QMouseEvent* event) {
    if (!m_currentTool->onMousePress(event)) {
		ZoomableGraphicsView::mousePressEvent(event);
    }		
}

void Editor::mouseMoveEvent(QMouseEvent* event) {
	if (!m_currentTool->onMouseMove(event)) {
		ZoomableGraphicsView::mouseMoveEvent(event);
    }
}

void Editor::mouseReleaseEvent(QMouseEvent* event) {
	if (!m_currentTool->onMouseRelease(event)) {
		ZoomableGraphicsView::mouseReleaseEvent(event);
    }
}

void Editor::setStatusBar(QStatusBar* statusBar) {
	m_statusBar = statusBar;
}


void Editor::zoomIn() {
	scale(1.2, 1.2);	
}

void Editor::zoomOut() {
	scale(0.8, 0.8);	
}

QGraphicsLineItem* Editor::addTracingIndicator()
{
    QGraphicsLineItem* line = new QGraphicsLineItem(0, 0, 0, 0);
    QPen pen(Qt::gray);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    line->setPen(pen);
    line->setVisible(false);
    m_scene->addItem(line);
	return line;
}

void Editor::showTracingIndicator() {
	if (m_tracingIndicator) {
		m_tracingIndicator->setVisible(true);
	}
}
void Editor::hideTracingIndicator() {
	if (m_tracingIndicator) {
		m_tracingIndicator->setVisible(false);
	}
}

void Editor::toggleLayerVisibility(LinkSide side, bool visible) {
	auto layer = m_layers[side];
    layer->setVisible(visible);
}

void Editor::showStatusMessage(const QString& message)
{	
    if (m_statusBar) {
        m_statusBar->showMessage(message);
    }
}

void Editor::clean() {
    m_undoStack.clear();
    TrackGraph::setTrackGraphCount(0);
    Link::setLinkCount(0);
    Node::setNodeCount(0);
    NotesTool::setNoteCount(0);
    Component::setComponentCount(0);

    // remove all items
    for (auto it = m_layers.constBegin(); it != m_layers.constEnd(); ++it) {
        QList<QGraphicsItem*> childItems = it.value()->childItems();
        for (QGraphicsItem* item : childItems) {
            m_scene->removeItem(item);
        }
    }

    m_guideTool->clear();
    m_trackDrawingTool->clean();
    
    // notify all listeners about the scene clean
    CommunicationHub::instance().publish(HubEvent::SCENE_CLEAN, nullptr);
}


void Editor::setCurrentSide(LinkSide side) {
	m_colorBox->setSide(side);
	m_currentSide = side;	
}

void Editor::setCurrentTool(IEditorTool *tool) {
	m_currentTool->exitMode();
	m_currentTool = tool;
    m_currentTool->enterMode();
}

void Editor::enterTrackMode() {
	m_state = DrawingState::TRACKS;
	setCurrentTool(m_trackDrawingTool);
}

void Editor::enterComponentMode() {
	m_state = DrawingState::IC;
	setCurrentTool(m_componentDrawingTool);
}

void Editor::enterNotesMode() {
	m_state = DrawingState::NOTES;
	setCurrentTool(m_notesTool);
}

TrackDrawingTool *Editor::getTrackDrawingTool() {
    return m_trackDrawingTool;
}

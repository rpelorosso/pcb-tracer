#include "TrackDrawingTool.h"
#include "actions/AddTrack.h"
#include "actions/DeleteTrack.h"
#include "actions/AssignSideToTrack.h"
#include "Component.h"
#include "TypeChecks.h"

TrackDrawingTool::TrackDrawingTool(Editor* editor)
    : m_editor(editor),
      m_drawingLineFrom(nullptr),
      m_drawingLineFromPos(),
      m_drawing(false),
      m_selectedItem(nullptr),
      m_highlighted_sub_circuit(0)
{
}

void TrackDrawingTool::enterMode()
{
    m_editor->setCursor(Qt::CrossCursor);
    qDebug() << "Entering track drawing mode";
}

bool TrackDrawingTool::onMousePress(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (!(event->modifiers() & Qt::ShiftModifier))
        {
            QGraphicsItem* item = m_editor->itemAt(event->pos());
            if (item && dynamic_cast<Link*>(item))
            {
                m_selectedItem = item;
                m_editor->showStatusMessage(QString("Selected track %1").arg((dynamic_cast<Link*>(m_selectedItem))->m_id));
            }
            else
            {
                startDrawing(event);
                m_selectedItem = nullptr;
                toggleHighlightSubCircuit(m_highlighted_sub_circuit, false);
            }
        }
        else
        {
            closeDrawing(event);
            return true; // to prevent sending this event to parent
        }
    }
    return false;
}

bool TrackDrawingTool::onMouseMove(QMouseEvent* event)
{
    if (m_editor->m_tracingIndicator && !m_drawingLineFromPos.isNull())
    {
        QPointF mousePos = m_editor->mapToScene(event->pos());
        qreal x0 = m_drawingLineFromPos.x();
        qreal y0 = m_drawingLineFromPos.y();
        qreal x1 = mousePos.x();
        qreal y1 = mousePos.y();

        if (event->modifiers() & Qt::ControlModifier)
        {
            //QPointF snapped = snapToNearest45Degrees(x0, y0, x1, y1);
            //x1 = snapped.x();
            //y1 = snapped.y();
        }

        m_editor->m_tracingIndicator->setLine(x0, y0, x1, y1);
        m_lastDraggedPosition = QPointF(x1, y1);
    }
    return false;
}

bool TrackDrawingTool::onMouseDoubleClick(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QGraphicsItem* item = m_editor->itemAt(event->pos());
        if (item && dynamic_cast<Link*>(item))
        {
            qDebug() << "double click on" << item;
            toggleHighlightSubCircuit(m_highlighted_sub_circuit, false);
            toggleHighlightSubCircuit(dynamic_cast<Link*>(item)->m_graphId, true);
        }
    }
    return true;
}

bool TrackDrawingTool::onKeyPress(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
    {
        m_editor->showTracingIndicator();
    }
    return true;
}

bool TrackDrawingTool::onKeyRelease(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
    {
        m_editor->hideTracingIndicator();
    }

    if (m_selectedItem && dynamic_cast<Link*>(m_selectedItem))
    {
        Link* selectedLink = dynamic_cast<Link*>(m_selectedItem);
        if (event->key() == Qt::Key_F || event->key() == Qt::Key_B || event->key() == Qt::Key_W)
        {
            if (event->key() == Qt::Key_F)
            {
                m_editor->setCurrentSide(LinkSide::FRONT);
            }
            else if (event->key() == Qt::Key_B)
            {
                m_editor->setCurrentSide(LinkSide::BACK);
                qDebug() << "steo back";
            }
            else
            {
                m_editor->setCurrentSide(LinkSide::WIP);
            }
            AssignSideToTrackMeta meta{selectedLink->m_id, m_editor->m_currentSide};
            m_editor->m_undoStack.push(new AssignSideToTrack(meta));
            m_editor->showStatusMessage(QString("Track moved to side: %1").arg(static_cast<int>(m_editor->m_currentSide)));
        }
        else if (event->key() == Qt::Key_Plus)
        {
            //ChangeTrackWidthMeta meta(selectedLink->id(), 1);
            //m_editor->undoStack->push(new ChangeTrackWidth(m_editor, meta));
        }
        else if (event->key() == Qt::Key_Minus)
        {
            //ChangeTrackWidthMeta meta(selectedLink->id(), -1);
            //m_editor->undoStack->push(new ChangeTrackWidth(m_editor, meta));
        }
        else if (event->key() == Qt::Key_Delete)
        {
            DeleteTrackMeta meta{selectedLink->m_id};
            m_editor->m_undoStack.push(new DeleteTrack(m_editor, meta));
        }
    }
    else
    {
        if (event->key() == Qt::Key_F)
        {
            //m_editor->m_currentSide = LinkSide::FRONT;
            m_editor->setCurrentSide(LinkSide::FRONT);
                }
        else if (event->key() == Qt::Key_B)
        {
            //m_editor->m_currentSide = LinkSide::BACK;
            m_editor->setCurrentSide(LinkSide::BACK);
        }
        else if (event->key() == Qt::Key_W)
        {
            //m_editor->m_currentSide = LinkSide::WIP;
            m_editor->setCurrentSide(LinkSide::WIP);

        }
        //m_editor->showStatusMessage(QString("Active track side: %1").arg(static_cast<int>(m_editor->m_currentSide)));
    }

    //m_editor->color_box->setSide(m_editor->m_currentSide);
    return true;
}

void TrackDrawingTool::startDrawing(QMouseEvent* event)
{
    m_drawing = true;
    m_drawingLineFrom = m_editor->itemAt(event->pos());

    /*
    if (m_drawing_line_from && dynamic_cast<ImageLayer*>(m_drawing_line_from))
    {
        m_drawing_line_from = nullptr;
    }
    */

    m_drawingLineFromPos = m_editor->mapToScene(event->pos());
}

void TrackDrawingTool::closeDrawing(QMouseEvent* event)
{
    m_drawing = false;
    QGraphicsItem* item = m_editor->itemAt(event->pos());
    

    qDebug() << "drawing started from position:" << m_drawingLineFromPos;
    qDebug() << "drawing started from item:" << m_drawingLineFrom;

    if (!isDynamicCastableToAny<Node, Link, Pad>(item))
    {
        item = nullptr;
        qDebug() << "drawing stopped at" << item;
    } 
    
    if (dynamic_cast<Node*>(item) && dynamic_cast<Node*>(m_drawingLineFrom) && m_drawingLineFrom && dynamic_cast<Node*>(item)->m_id == dynamic_cast<Node*>(m_drawingLineFrom)->m_id)
    {
        item = nullptr;
    }

        
        if (dynamic_cast<Link*>(item) && (dynamic_cast<Link*>(item))->m_side != m_editor->m_currentSide)
        {
            m_editor->hideTracingIndicator();
            QMessageBox::StandardButton reply = QMessageBox::question(
                m_editor,
                "Connect track on different sides",
                QString("Do you want to connect this %1 track to track on the %2?")
                    .arg(static_cast<int>(m_editor->m_currentSide))
                    .arg(static_cast<int>(dynamic_cast<Link*>(item)->m_side)),
                QMessageBox::Yes | QMessageBox::No
            );
            if (reply == QMessageBox::No)
            {
                item = nullptr;
            }
        }

    QPointF startingPoint = m_drawingLineFromPos;
    QPointF endingPoint = m_lastDraggedPosition;

    std::optional<int> fromNodeId; 
    
    if (dynamic_cast<Node*>(m_drawingLineFrom))
    {
        fromNodeId = dynamic_cast<Node*>(m_drawingLineFrom)->m_id;
    }
    else if (dynamic_cast<Pad*>(m_drawingLineFrom))
    {
        fromNodeId = dynamic_cast<Pad*>(m_drawingLineFrom)->m_id;
    }

    TrackCreationMeta meta{fromNodeId, startingPoint, endingPoint, item, m_editor->m_currentSide};

    AddTrack* action = new AddTrack(meta);
    Editor::instance()->m_undoStack.push(action);
    //m_drawingLineFromPos = endingPoint;
    //m_drawingLineFrom = action->m_to_node;
}

void TrackDrawingTool::toggleHighlightSubCircuit(int graphId, bool isHighlighted)
{
    if (graphId)
    {
        QList<QGraphicsItem*> items = m_editor->scene()->items();
        for (QGraphicsItem* item : items)
        {
            Link* link = dynamic_cast<Link*>(item);
            if (link && link->m_graphId == graphId)
            {
                link->setHighlighted(isHighlighted);
            }
        }

        if (isHighlighted)
        {
            m_highlighted_sub_circuit = graphId;
        }
        else
        {
            m_highlighted_sub_circuit = 0;
        }
    }
}

void TrackDrawingTool::clean() {
    m_drawingLineFrom = nullptr;    
    m_highlighted_sub_circuit = 0;
}
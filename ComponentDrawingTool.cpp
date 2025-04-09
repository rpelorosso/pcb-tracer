#include <QPointF>
#include <QInputDialog>
#include <QGraphicsScene>
#include <cmath>
#include "ComponentDrawingTool.h"
#include "Editor.h"
#include "Node.h"
#include "Component.h"
#include "actions/AddComponent.h"

//#include "AddComponent.h"

ComponentDrawingTool::ComponentDrawingTool() {}

void ComponentDrawingTool::enterMode()
{
    Editor::instance()->setCursor(Qt::CrossCursor);
    clearPoints();
}

bool ComponentDrawingTool::onMousePress(QMouseEvent* event)
{
    QPointF mousePos = Editor::instance()->mapToScene(event->pos());
    m_lastClickPos = mousePos;

    if (event->button() == Qt::LeftButton)
    {
        if (event->modifiers() & Qt::ShiftModifier)
        {
            if (!m_points.isEmpty() && m_points.last().type() == QVariant::PointF)            
            {
                QPointF lastPoint = m_points.takeLast().toPointF();
                QVector<QPointF> pointPair = {lastPoint, mousePos};
                m_points.append(QVariant::fromValue(pointPair));
            }
            else
            {
                m_points.append(QVariant::fromValue(mousePos));
            }
        }
        else
        {
            m_points.append(QVariant::fromValue(mousePos));
        }

        // add phantom pad
        PhantomPad* phantomPad = new PhantomPad(QString("Phantom_%1").arg(m_phantomPads.size()), mousePos);
        m_phantomPads.append(phantomPad);
        Editor::instance()->scene()->addItem(phantomPad);
    }
    return true;
}

bool ComponentDrawingTool::onKeyPress(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        clearPoints();
        return true;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (!m_points.isEmpty())
        {
            createComponentNodes();
        }
        else
        {
            qDebug() << "No points entered. Add points before creating a component.";
        }
    }
    if (event->key() == Qt::Key_Shift)
    {
        Editor::instance()->showTracingIndicator();
    }
    return true;
}

bool ComponentDrawingTool::onKeyRelease(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
    {
        Editor::instance()->hideTracingIndicator();
    }
    return true;
}

bool ComponentDrawingTool::onMouseMove(QMouseEvent* event)
{
    if (Editor::instance()->m_tracingIndicator && !m_lastClickPos.isNull())
    {
        QPointF mousePos = Editor::instance()->mapToScene(event->pos());
        Editor::instance()->m_tracingIndicator->setLine(
            m_lastClickPos.x(),
            m_lastClickPos.y(),
            mousePos.x(),
            mousePos.y()
        );
    }
    return false;
}

void ComponentDrawingTool::createComponentNodes()
{
    if (m_points.isEmpty())
    {
        qDebug() << "No points to create component nodes from.";
        return;
    }

    bool ok;
    QString componentName = QInputDialog::getText(Editor::instance(), "Component Name", "Enter component name:", QLineEdit::Normal, QString(), &ok);
    if (!ok || componentName.isEmpty())
    {
        return;
    }

    int singlePoints = 0;
    int pointPairs = 0;
    for (const QVariant& point : m_points)
    {
        if (point.canConvert<QPointF>())
        {
            singlePoints++;
        }
        else if (point.canConvert<QVector<QPointF>>())
        {
            pointPairs++;
        }
    }

    int totalPins;
    if (pointPairs > 0)
    {
        totalPins = QInputDialog::getInt(Editor::instance(), "Total Pins", "Enter total number of pins:", 0, 0, 1000, 1, &ok);
        if (!ok || totalPins <= 0)
        {
            return;
        }
    }
    else
    {
        totalPins = singlePoints;
    }

    int pinsForPairs = totalPins - singlePoints;
    int pinsPerPair = pointPairs > 0 ? std::ceil(static_cast<double>(pinsForPairs) / pointPairs) : 0;

    std::vector<Pad*> pads;
    int padCount = 1;

    for (const QVariant& item : m_points)
    {
        if (item.canConvert<QPointF>())
        {
            QPointF point = item.toPointF();
            QString padName = QString("%1 Pin %2").arg(componentName).arg(padCount);
            int padId = Node::genNodeId();
            Pad* pad = new Pad(padName, padId, point, padCount);
            pads.push_back(pad);
            padCount++;
        }
        else if (item.canConvert<QVector<QPointF>>())
        {
            QVector<QPointF> pointPair = item.value<QVector<QPointF>>();
            if (pointPair.size() == 2)
            {
                QPointF start = pointPair[0];
                QPointF end = pointPair[1];
                for (int i = 0; i < pinsPerPair; ++i)
                {
                    double t = pinsPerPair > 1 ? static_cast<double>(i) / (pinsPerPair - 1) : 0;
                    double x = start.x() + t * (end.x() - start.x());
                    double y = start.y() + t * (end.y() - start.y());
                    QString padName = QString("%1 Pin %2").arg(componentName).arg(padCount);
                    int padId = Node::genNodeId();
                    Pad* pad = new Pad(padName, padId, QPointF(x, y), padCount);
                    pads.push_back(pad);
                    padCount++;
                    if (padCount > totalPins)
                    {
                        break;
                    }
                }
            }
        }
        if (padCount > totalPins)
        {
            break;
        }
    }

    AddComponentMeta meta{.m_name=componentName,.m_pads=pads};
    AddComponent* action = new AddComponent(meta);

    // add the action to the undo stack
    Editor::instance()->m_undoStack.push(action);
    qDebug() << "Created component '" << componentName << "' with" << padCount - 1 << "pins";

    clearPoints();
}

void ComponentDrawingTool::removePhantomPads()
{
    for (PhantomPad* phantomPad : m_phantomPads)
    {
        Editor::instance()->scene()->removeItem(phantomPad);
        delete phantomPad;
    }
    m_phantomPads.clear();
}

void ComponentDrawingTool::clearPoints()
{
    Editor::instance()->hideTracingIndicator();
    m_points.clear();
    removePhantomPads();
}

void ComponentDrawingTool::exitMode() {
    clearPoints();
}

QVector<QVariant> ComponentDrawingTool::getPoints() const
{
    return m_points;
}
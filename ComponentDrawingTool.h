#ifndef COMPONENTDRAWINGTOOL_H
#define COMPONENTDRAWINGTOOL_H

#include <QPointF>
#include <QVector>
#include <QVariant>
#include "Component.h"
#include "Editor.h"

class ComponentDrawingTool: public IEditorTool
{
public:
    ComponentDrawingTool();
    void enterMode();
    void exitMode();
    bool onMousePress(QMouseEvent* event);
    bool onKeyPress(QKeyEvent* event);
    bool onKeyRelease(QKeyEvent* event);
    bool onMouseMove(QMouseEvent* event);
    void createComponentNodes();
    void removePhantomPads();
    void clearPoints();
    QVector<QVariant> getPoints() const;

private:
    QPointF m_lastClickPos;
    QVector<PhantomPad*> m_phantomPads;
    QVector<QVariant> m_points;
};

#endif // COMPONENTDRAWINGTOOL_H
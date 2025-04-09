#pragma once

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QMouseEvent>
#include <memory>

class Editor;
class Link;
class Node;
enum class DrawingState;
enum class LinkSide;
class Component;
class Pad;
class PhantomPad;


class GuideTool {
public:
    GuideTool();
    ~GuideTool();
    void enterMode();
    void clear();
    void onMousePress(QMouseEvent* event);
    void onKeyPress(QKeyEvent* event);
    void onKeyRelease(QKeyEvent* event);
    void onMouseMove(QMouseEvent* event);
    void setLayerOpacity(LinkSide side, qreal alpha);
    void setImageLayer(LinkSide side, const QString& imagePath);

private:

};
#ifndef IMAGETRANSFORMTOOL_H
#define IMAGETRANSFORMTOOL_H

#include "IEditorTool.h"
#include <QGraphicsEllipseItem>
#include <QMetaObject>
#include <QPointF>
#include <array>
#include <functional>

class ImageLayer;

struct HandleCallbacks {
    std::function<void(int)> onMoved;
    std::function<void(int)> onDragStarted;
    std::function<void(int)> onDragFinished;
};

class TransformHandle : public QGraphicsEllipseItem {
public:
    TransformHandle(int index, HandleCallbacks callbacks);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    int m_index;
    bool m_dragging = false;
    HandleCallbacks m_callbacks;
};

// 0-3: corner handles, 4-7: edge handles (top, right, bottom, left)
static constexpr int HANDLE_COUNT = 8;
static constexpr int EDGE_PAIRS[4][2] = {{0,1}, {1,2}, {2,3}, {3,0}};

class ImageTransformTool : public IEditorTool {
public:
    ImageTransformTool(Editor* editor, ImageLayer* layer);
    ~ImageTransformTool();

    void enterMode() override;
    void exitMode() override;
    bool onKeyPress(QKeyEvent* event) override;

    void handleMoved(int index);
    void handleDragStarted(int index);
    void handleDragFinished(int index);

private:
    void createHandles();
    void removeHandles();
    void updateHandlePositions();

    ImageLayer* m_layer;
    std::array<TransformHandle*, HANDLE_COUNT> m_handles = {};
    std::array<QPointF, 4> m_cornersBeforeDrag;
    bool m_updatingHandles = false;
    QMetaObject::Connection m_undoConnection;
};

#endif // IMAGETRANSFORMTOOL_H

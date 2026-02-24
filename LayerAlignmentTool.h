#pragma once

#include "IEditorTool.h"
#include <QPointF>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QPolygonF>
#include <vector>

class Editor;

class LayerAlignmentTool : public IEditorTool {
public:
    LayerAlignmentTool(Editor* editor);

    void enterMode() override;
    void exitMode() override;
    bool onMousePress(QMouseEvent* event) override;
    bool onKeyPress(QKeyEvent* event) override;

private:
    enum class Phase { PICK_FRONT, PICK_BACK };

    struct Marker {
        QGraphicsEllipseItem* circle;
        QGraphicsSimpleTextItem* label;
    };

    void addMarker(const QPointF& scenePos, int index, const QColor& color);
    bool computeAffineTransform(const QPolygonF& srcLocal, const QPolygonF& dstLocal, QTransform& result);
    void applyAlignment();
    void cancel();
    void clearMarkers();

    Phase m_phase;
    std::vector<QPointF> m_frontPoints;
    std::vector<QPointF> m_backPoints;
    std::vector<Marker> m_markers;
};

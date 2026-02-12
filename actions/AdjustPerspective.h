#pragma once

#include <QUndoCommand>
#include <QPointF>
#include <array>

class ImageLayer;

struct AdjustPerspectiveMeta {
    int layerId;
    std::array<QPointF, 4> oldCorners;
    std::array<QPointF, 4> newCorners;
};

class AdjustPerspective : public QUndoCommand {
public:
    AdjustPerspective(const AdjustPerspectiveMeta& meta);

    void undo() override;
    void redo() override;

private:
    void applyCorners(const std::array<QPointF, 4>& corners);

    AdjustPerspectiveMeta m_meta;
    ImageLayer* m_layer;
};

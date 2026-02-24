#pragma once

#include <QUndoCommand>
#include <QTransform>

class ImageLayer;

struct AlignLayerMeta {
    int layerId;
    QTransform oldTransform;
    QTransform newTransform;
};

class AlignLayer : public QUndoCommand {
public:
    AlignLayer(const AlignLayerMeta& meta);

    void undo() override;
    void redo() override;

private:
    AlignLayerMeta m_meta;
    ImageLayer* m_layer;
};

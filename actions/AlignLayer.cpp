#include "AlignLayer.h"
#include "../Editor.h"
#include "../ImageLayer.h"

AlignLayer::AlignLayer(const AlignLayerMeta& meta)
    : QUndoCommand()
    , m_meta(meta)
{
    m_layer = Editor::instance()->findItemByIdAndClass<ImageLayer>(m_meta.layerId);
}

void AlignLayer::undo()
{
    if (m_layer) {
        m_layer->setTransform(m_meta.oldTransform);
    }
}

void AlignLayer::redo()
{
    if (m_layer) {
        m_layer->setTransform(m_meta.newTransform);
    }
}

#include "AdjustPerspective.h"
#include "../Editor.h"
#include "../ImageLayer.h"

AdjustPerspective::AdjustPerspective(const AdjustPerspectiveMeta& meta)
    : QUndoCommand()
    , m_meta(meta)
{
    m_layer = Editor::instance()->findItemByIdAndClass<ImageLayer>(m_meta.layerId);
}

void AdjustPerspective::undo() {
    applyCorners(m_meta.oldCorners);
}

void AdjustPerspective::redo() {
    applyCorners(m_meta.newCorners);
}

void AdjustPerspective::applyCorners(const std::array<QPointF, 4>& corners) {
    if (!m_layer) return;
    m_layer->m_corners = corners;
    m_layer->applyPerspectiveTransform();
}

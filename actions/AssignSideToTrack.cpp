#include "AssignSideToTrack.h"
#include "../ZoomableGraphicsView.h"
#include "../Link.h"
#include "../Node.h"
#include "../Editor.h"

AssignSideToTrack::AssignSideToTrack(const AssignSideToTrackMeta& meta)
    : QUndoCommand(), m_meta(meta)
{
    // store meta data
    m_link = Editor::instance()->findItemByIdAndClass<Link>(m_meta.m_linkId);
    m_oldSide = m_link->m_side;
}

void AssignSideToTrack::undo()
{
    // put back the old side
    m_link->setSide(m_oldSide);
    m_link->refresh();
}

void AssignSideToTrack::redo()
{
    m_link->setSide(m_meta.m_side);
    m_link->refresh();
}

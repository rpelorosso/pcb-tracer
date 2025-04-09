#include "MoveNode.h"
#include "../Editor.h"
#include <QVariantMap>

MoveNode::MoveNode(const MoveNodeMeta& meta)
    : QUndoCommand()
    , m_meta(meta)
{
    m_node = Editor::instance()->findItemByIdAndClass<Node>(m_meta.nodeId);
}

void MoveNode::undo()
{
    m_node->setPos(m_meta.sourcePosition);
    m_node->updateLinks();
}

void MoveNode::redo()
{
    m_node->setPos(m_meta.targetPosition);
    m_node->updateLinks();
}

/*
QVariantMap MoveNode::toDict() const
{
    QVariantMap dict;
    dict["nodeId"] = m_meta.nodeId;
    dict["sourcePosition"] = m_meta.sourcePosition;
    dict["targetPosition"] = m_meta.targetPosition;
    return dict;
}
    */
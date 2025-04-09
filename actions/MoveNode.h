#pragma once

#include <QUndoCommand>
#include <QPointF>
#include "../Node.h"

struct MoveNodeMeta {
    int nodeId;
    QPointF sourcePosition;
    QPointF targetPosition;
};

class MoveNode : public QUndoCommand {
public:
    MoveNode(const MoveNodeMeta& meta);

    void undo() override;
    void redo() override;

    QVariantMap toDict() const;

private:
    MoveNodeMeta m_meta;
    Node* m_node;
};
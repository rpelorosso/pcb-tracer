#pragma once

#include <QGraphicsView>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QUndoCommand>
#include <QUndoStack>
#include <tuple>
#include <memory>
#include "../enums.h"

class ZoomableGraphicsView;
class Link;
class Node;
class Editor;

struct AssignSideToTrackMeta {
    int m_linkId;
    LinkSide m_side;
};

class AssignSideToTrack : public QUndoCommand {
public:
    AssignSideToTrack(const AssignSideToTrackMeta& meta);

    void undo() override;
    void redo() override;
    QVariantMap toDict() const;

private:
    AssignSideToTrackMeta m_meta;
    QGraphicsScene* m_scene;
    Link* m_link;
    LinkSide m_oldSide;
};
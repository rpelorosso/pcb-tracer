#ifndef ADDTRACK_H
#define ADDTRACK_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QPointF>
#include <map>
#include <vector>
#include "../Link.h"
#include "../Node.h"
#include "../ZoomableGraphicsView.h"
#include "../Editor.h"
#include "../enums.h"

struct TrackCreationMeta {
    std::optional<int> m_from_node_id;
    QPointF m_from_position;
    QPointF m_to_position;
    QGraphicsItem* m_to_item;
    LinkSide m_side;
};

struct GraphIdChange {
    int m_link_id;
    int m_old_graph_id;
    int m_new_graph_id;
};

class AddTrack : public QUndoCommand {
public:
    AddTrack(const TrackCreationMeta& meta);
    ~AddTrack();

    void undo() override;
    void redo() override;

    Node* m_from_node;
    Node* m_to_node;

private:
    void calculateGraphIds();

    bool m_created_from_node;
    bool m_created_to_node;
    bool m_created_split_link;
    QGraphicsScene* m_scene;
    //QGraphicsScene* m_graphics_scene;
    TrackCreationMeta m_meta;
    std::map<QString, QVariant> m_split_link_meta;
    bool m_is_split_link;
    std::vector<GraphIdChange> m_old_graph_ids;
    std::vector<GraphIdChange> m_new_graph_ids;

    int m_link_id;
    Link* m_link;
};

#endif // ADDTRACK_H
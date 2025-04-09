#pragma once

#include <QUndoCommand>
#include <QGraphicsScene>
#include <vector>
#include <optional>
#include <unordered_set>
#include <queue>

class ZoomableGraphicsView;
class Link;
class Node;
class Editor;
struct GraphIdChange;

struct DeleteTrackMeta {
    int m_linkId;
};

class DeleteTrack : public QUndoCommand {
public:
    DeleteTrack(ZoomableGraphicsView* scene, const DeleteTrackMeta& meta);
    void undo() override;
    void redo() override;

private:
    std::tuple<bool, std::vector<Link*>, std::optional<std::vector<Link*>>> checkGraphSplit();
    std::tuple<std::unordered_set<Node*>, std::unordered_set<Link*>> bfs(Node* startNode, int ignoreLink);
    void calculateGraphIds();

    Editor* m_editor;
    QGraphicsScene* m_scene;
    DeleteTrackMeta m_meta;
    std::vector<GraphIdChange> m_graphIdChanges;
    Link* m_link;
    Node* m_fromNode;
    Node* m_toNode;
    std::tuple<bool, std::vector<Link*>, std::optional<std::vector<Link*>>> m_splitAnalysis;
    std::vector<Node*> m_nodesToDelete;
    std::vector<Node*> m_nodesToUpdate;
    bool m_deleteToNode;
    bool m_deleteFromNode;
};

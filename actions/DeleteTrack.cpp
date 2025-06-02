#include "DeleteTrack.h"
#include "../Editor.h"
#include "../Link.h"
#include "../Node.h"
#include "../ZoomableGraphicsView.h"
#include "../Link.h"
#include "AddTrack.h" // GraphIdChange
#include "../Component.h"

DeleteTrack::DeleteTrack(ZoomableGraphicsView* scene, const DeleteTrackMeta& meta)
    : m_editor(Editor::instance()), m_scene(scene->scene()), m_meta(meta) {
    
    m_link = m_editor->findItemByIdAndClass<Link>(m_meta.m_linkId);
    m_fromNode = m_link->fromNode();
    m_toNode = m_link->toNode();

    m_splitAnalysis = checkGraphSplit();

    m_deleteToNode = (m_link->toNode()->getGrade() == 1) && (dynamic_cast<Pad*>(m_link->toNode()) == nullptr);
    m_deleteFromNode = (m_link->fromNode()->getGrade() == 1) && (dynamic_cast<Pad*>(m_link->fromNode()) == nullptr);

    calculateGraphIds();
}

void DeleteTrack::undo() {
    m_link->setSide(m_link->m_side); // so it gets added back to the scene
    if (m_deleteToNode) {
        m_toNode->setSide(LinkSide::NODE); // setting the side adds it to the scene
    }
    if (m_deleteFromNode) {
        m_fromNode->setSide(LinkSide::NODE);
    }
    
    m_link->setFromNode(m_fromNode);
    m_link->setToNode(m_toNode);

    m_fromNode->notifyLinkChanges();
    m_toNode->notifyLinkChanges();

    for (const auto& change : m_graphIdChanges) {
        Link* linkToChange = m_editor->findItemByIdAndClass<Link>(change.m_link_id);
        if (linkToChange) {
            linkToChange->setGraphId(change.m_old_graph_id);
            linkToChange->updateTextItem(QString::number(change.m_old_graph_id));
        }
    }
}

void DeleteTrack::redo() {

    if (m_deleteToNode) {
        m_toNode->willBeDeleted();
        m_scene->removeItem(m_toNode);
    } else {
        m_toNode->notifyLinkChanges();
    }

    if (m_deleteFromNode) {
        m_fromNode->willBeDeleted();
        m_scene->removeItem(m_fromNode);
    } else {
        m_fromNode->notifyLinkChanges();
    }

    for (const auto& change : m_graphIdChanges) {
        Link* linkToChange = m_editor->findItemByIdAndClass<Link>(change.m_link_id);
        if (linkToChange) {
            linkToChange->setGraphId(change.m_new_graph_id);
            linkToChange->updateTextItem(QString::number(change.m_new_graph_id));
        }
    }
    m_link->remove();

}

std::tuple<bool, std::vector<Link*>, std::optional<std::vector<Link*>>> DeleteTrack::checkGraphSplit() {
    auto [visitedNodes, visitedLinks] = bfs(m_fromNode, m_link->m_id);

    bool isSplit = visitedNodes.find(m_toNode) == visitedNodes.end();
    std::vector<Link*> graph;
    std::vector<Link*> graph1;

    for (Link* link : visitedLinks) {
        if (link->m_id != m_link->m_id) {
            graph1.push_back(link);
        }
    }

    if (isSplit) {
        std::vector<Link*> splitItems;
        for (auto item : m_scene->items()) {
            if (auto link = dynamic_cast<Link*>(item)) {
                if (link->m_graphId == m_link->m_graphId && link->m_id != m_link->m_id) {
                    splitItems.push_back(link);
                }
            }
        }

        std::vector<Link*> graph2;
        std::set_difference(splitItems.begin(), splitItems.end(), 
                            visitedLinks.begin(), visitedLinks.end(),
                            std::back_inserter(graph2));

        if (graph1.empty()) {
            isSplit = false;
            graph = graph2;
        }

        if (graph2.empty()) {
            isSplit = false;
            graph = graph1;
        }

        if (isSplit) {
            return {true, graph1, graph2};
        }
    } else {
        graph = std::vector<Link*>(visitedLinks.begin(), visitedLinks.end());
    }

    return {false, graph, std::nullopt};
}

std::tuple<std::unordered_set<Node*>, std::unordered_set<Link*>> DeleteTrack::bfs(Node* startNode, int ignoreLink) {
    std::unordered_set<Node*> visitedNodes;
    std::unordered_set<Link*> visitedLinks;
    std::queue<Node*> queue;

    queue.push(startNode);

    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop();

        if (visitedNodes.find(node) == visitedNodes.end()) {
            visitedNodes.insert(node);
            for (Link* link : node->getLinks()) {
                if (link->m_id != ignoreLink) {
                    visitedLinks.insert(link);
                    Node* nextNode = (link->fromNode() == node) ? link->toNode() : link->fromNode();
                    if (visitedNodes.find(nextNode) == visitedNodes.end()) {
                        queue.push(nextNode);
                    }
                }
            }
        }
    }

    return {visitedNodes, visitedLinks};
}

void DeleteTrack::calculateGraphIds() {
    auto [isSplit, graph, optionalGraph2] = m_splitAnalysis;

    if (isSplit) {
        int newGraphId = TrackGraph::genTrackGraphId();
        for (Link* link : graph) {
            m_graphIdChanges.push_back({link->m_id, link->m_graphId, newGraphId});
        }
    }
}

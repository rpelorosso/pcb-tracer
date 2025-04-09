#include "AddTrack.h"
#include "../Editor.h"

AddTrack::AddTrack(const TrackCreationMeta& meta)
    : m_created_from_node(false), m_created_to_node(false), m_created_split_link(false),
      m_meta(meta),
      m_is_split_link(false),
      m_link(nullptr), m_from_node(nullptr), m_to_node(nullptr) {

    m_scene = Editor::instance()->scene();

    // gen id
    m_link_id = Link::genLinkId();
    m_link = new Link(m_link_id);

    // if FROM node is given, use it, else create a new node at the given position
    if (m_meta.m_from_node_id.has_value()) {
        m_from_node = Editor::instance()->findItemByIdAndClass<Node>(m_meta.m_from_node_id.value());
    } else {
        m_from_node = new Node(Node::genNodeId());
        m_from_node->setPosition(m_meta.m_from_position);
        m_created_from_node = true;
    }

    // if TO node is given, use it, else create a new node at the given position
    if (m_meta.m_to_item && m_meta.m_to_item->type() != Link::Type) {
        m_to_node = dynamic_cast<Node*>(m_meta.m_to_item);
    } else {
        m_to_node = new Node(Node::genNodeId());
        m_to_node->setPosition(m_meta.m_to_position);
        m_created_to_node = true;
        // if TO is a link, we need to split the clicked link
        if (m_meta.m_to_item && m_meta.m_to_item->type() == Link::Type) {
            m_split_link_meta["new_link_a"] = QVariant::fromValue(new Link(Link::genLinkId()));
            m_split_link_meta["new_link_b"] = QVariant::fromValue(new Link(Link::genLinkId()));
            m_split_link_meta["delete_link"] = QVariant::fromValue((Link*)m_meta.m_to_item);
            m_is_split_link = true; // maybe the map could be an optional so we can avoid using this
            qDebug() << "is a link" << m_split_link_meta;
            qDebug() <<  "meta.m_to_item" << m_meta.m_to_item;

        }
    }

    m_link->setSide(m_meta.m_side);

    calculateGraphIds();

    qDebug() << "calculated:" << m_new_graph_ids.size();
}

AddTrack::~AddTrack() {
    qDebug() << "AddTrack::~AddTrack()";
    if (m_created_from_node) {
        delete m_from_node;
    }
    if (m_created_to_node) {
        delete m_to_node;
    }
    delete m_link;
}

void AddTrack::undo() {
    // remove nodes from the scene
    if (m_created_from_node) {
        m_from_node->willBeDeleted();
        m_scene->removeItem(m_from_node);
    }

    if (m_created_to_node) {
        m_to_node->willBeDeleted();
        m_scene->removeItem(m_to_node);
    }

    // remove links from nodes (update this to use link.remove)
    m_from_node->removeLink(m_link);
    m_to_node->removeLink(m_link);

    // remove the link from the scene (update this to use link.remove)
    m_scene->removeItem(m_link);

    // remove new segmented links if they were created
    if (!m_split_link_meta.empty()) {
        Link* new_link_a = m_split_link_meta["new_link_a"].value<Link*>();
        Link* new_link_b = m_split_link_meta["new_link_b"].value<Link*>();
        new_link_a->remove();
        new_link_b->remove();

        // re-add deleted link
        Link* deleted_link = m_split_link_meta["delete_link"].value<Link*>();
        deleted_link->setSide(deleted_link->m_side);
        deleted_link->setFromNode(m_split_link_meta["target_link_from_node"].value<Node*>());
        deleted_link->setToNode(m_split_link_meta["target_link_to_node"].value<Node*>());
    }

    for (const auto& item : m_old_graph_ids) {
        Link* link_to_change = Editor::instance()->findItemByIdAndClass<Link>(item.m_link_id);
        if (link_to_change) {
            link_to_change->setGraphId(item.m_old_graph_id);
            link_to_change->updateTextItem(QString::number(item.m_old_graph_id));
            qDebug() << "changing" << link_to_change;
        }
    }

    if (!m_created_from_node) {
        m_from_node->notifyLinkChanges();
    }
    if (!m_created_to_node) {
        m_to_node->notifyLinkChanges();
    }
}

void AddTrack::redo() {
    // configure new link
    m_link->setFromNode(m_from_node);
    m_link->setToNode(m_to_node);

    // add items to the scene
    m_link->setSide(m_meta.m_side); // setting parent adds to the scene

    if (m_created_from_node) {
        m_from_node->setPosition(m_meta.m_from_position);
        // note that setting the parent will add it to the scene so no need to call addItem
        m_from_node->setSide(LinkSide::NODE);
    }

    if (m_created_to_node) {
        m_to_node->setPosition(m_meta.m_to_position);
        // note that setting the parent will add it to the scene so no need to call addItem
        m_to_node->setSide(LinkSide::NODE);
    }

    // add links to nodes
    m_from_node->addLink(m_link);
    m_to_node->addLink(m_link);

    // configure new link
    m_link->setFromNode(m_from_node);
    m_link->setToNode(m_to_node);

    m_link->refresh();

    if (!m_split_link_meta.empty()) {
        Link* link_to_remove = m_split_link_meta["delete_link"].value<Link*>();
        m_split_link_meta["target_link_from_node"] = QVariant::fromValue(link_to_remove->fromNode());
        m_split_link_meta["target_link_to_node"] = QVariant::fromValue(link_to_remove->toNode());
        Link* new_link_a = m_split_link_meta["new_link_a"].value<Link*>();
        Link* new_link_b = m_split_link_meta["new_link_b"].value<Link*>();
        // 1- eliminar el link original
        link_to_remove->remove();

        // 2- linkear el nodo nuevo con los dos nuevos links
        new_link_a->setFromNode(link_to_remove->fromNode());
        new_link_a->setToNode(m_to_node);
        new_link_b->setFromNode(m_to_node);
        new_link_b->setToNode(link_to_remove->toNode());
        new_link_a->setSide(link_to_remove->m_side);
        new_link_b->setSide(link_to_remove->m_side);
    }

    // update graph ids
    for (const auto& item : m_old_graph_ids) {
        Link* link_to_change = Editor::instance()->findItemByIdAndClass<Link>(item.m_link_id);
        if (link_to_change) {
            link_to_change->setGraphId(item.m_new_graph_id);
        }
    }

    // both were created or existing, but either way they had their number of links changed
    m_to_node->notifyLinkChanges();
    m_from_node->notifyLinkChanges();
}

void AddTrack::calculateGraphIds() {
    
    if (m_from_node) {
        std::vector<int> a, b;

        for (const auto& link : m_from_node->getLinks()) {
            if (link->m_id != m_link->m_id) {
                a.push_back(link->m_graphId);
            }
        }

        for (const auto& link : m_to_node->getLinks()) {
            if (link->m_id != m_link->m_id) {
                b.push_back(link->m_graphId);
            }
        }

        qDebug() << "uno nodo con a =" << a << "a nodos con b =" << b;

        std::optional<int> max_graph_id, min_graph_id;

        if(!(a.empty() && b.empty())) {
            // we can rewrite this without using a temp array
            std::vector<int> combined;
            combined.insert(combined.end(), a.begin(), a.end());
            combined.insert(combined.end(), b.begin(), b.end());
            auto [min_it, max_it] = std::minmax_element(combined.begin(), combined.end());
            min_graph_id = *min_it;
            max_graph_id = *max_it;
        }

        if(!max_graph_id.has_value()) {
            max_graph_id = TrackGraph::genTrackGraphId();
        }


        if (min_graph_id.has_value() && !a.empty() && !b.empty() && min_graph_id.value() != max_graph_id.value()) {
            // updates the ones that have min_graph_id
            for (auto item : m_scene->items()) {
                if (auto link = dynamic_cast<Link*>(item)) {
                    if (link->m_graphId == min_graph_id) {
                        
                        m_old_graph_ids.push_back(GraphIdChange{//+
                            .m_link_id=link->m_id,//+
                            .m_old_graph_id=min_graph_id.value(),//+
                            .m_new_graph_id=max_graph_id.value()//+
                        });                         

                    }
                }
            }
        }


        m_link->setGraphId(max_graph_id.value());
        m_link->updateTextItem(QString::number(max_graph_id.value()));

        if(m_is_split_link) {
            Link* link_to_remove = m_split_link_meta["delete_link"].value<Link*>();

            m_old_graph_ids.push_back(GraphIdChange{ //+
                .m_link_id=m_split_link_meta["new_link_a"].value<Link*>()->m_id, //+
                .m_old_graph_id=m_split_link_meta["new_link_a"].value<Link*>()->m_graphId,
                .m_new_graph_id=max_graph_id.value()//+
            });  
            m_old_graph_ids.push_back(GraphIdChange{ //+
                .m_link_id=m_split_link_meta["new_link_b"].value<Link*>()->m_id, //+
                .m_old_graph_id=m_split_link_meta["new_link_b"].value<Link*>()->m_graphId,
                .m_new_graph_id=max_graph_id.value()//+
            });  
            qDebug() << "IS SPLIT";
            qDebug() << "m_old_graph_ids:";
            for (const auto& change : m_old_graph_ids) {
                qDebug() << "  Link ID:" << change.m_link_id
                         << "Old Graph ID:" << change.m_old_graph_id
                         << "New Graph ID:" << change.m_new_graph_id;
            }
 
            for (auto item : m_scene->items()) {
                if (auto link = dynamic_cast<Link*>(item)) {
                    if(link->m_graphId == m_split_link_meta["delete_link"].value<Link*>()->m_graphId) {

                        m_old_graph_ids.push_back(GraphIdChange{
                            .m_link_id=link->m_id,
                            .m_old_graph_id=link->m_graphId,
                            .m_new_graph_id=max_graph_id.value()
                        });  
            
                    }
                }
            }
        }


        /*
        for item in self.scene.items():
        if isinstance(item, Link) and hasattr(item, 'graph_id') and item.graph_id == self.splitLinkMeta["delete_link"].graph_id:
            self.new_graph_ids.append({"link_id": item.id, "new_graph_id": max_graph_id})
            self.old_graph_ids.append({"link_id": item.id, "old_graph_id": item.graph_id})
*/

        qDebug() << "##########################################";
        // print the content of m_new_graph_ids
        for (const auto& item : m_new_graph_ids) {
            qDebug() << "link_id =" << item.m_link_id << " old_graph_id =" << item.m_old_graph_id << " new_graph_id =" << item.m_new_graph_id;
        }

    }
        
}
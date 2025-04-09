#include "Node.h"
#include "Link.h"
#include "Editor.h"
#include "Component.h"
#include "actions/MoveNode.h"

int Node::node_count = 0;

Node::Node(int id) : QGraphicsEllipseItem(), m_id(id), m_size(0), m_side(LinkSide::FRONT), m_links()  {
    Editor* editor = Editor::instance();
    m_size = 20; // editor->padSize();

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);

    setOpacity(1);
    m_showOnHover = true;

    setColor(Color::NODE);

    if (m_showOnHover) {
        setZValue(0.1);
        setOpacity(0.001);
    }
    setAcceptHoverEvents(true);
}

void Node::notifyLinkChanges() {
    if (typeid(*this).name() != typeid(Pad).name()) {
        if (m_links.size() > 1) {
            CommunicationHub::instance().publish(HubEvent::NODE_MADE_MULTIPLE_LINKS, this);
        } else {
            CommunicationHub::instance().publish(HubEvent::NODE_MADE_SINGLE_LINK, this);
        }
    }
}

void Node::willBeDeleted() {
    CommunicationHub::instance().publish(HubEvent::NODE_DELETED, this);
}

void Node::addLink(Link* link) {
    if (link == nullptr) {
        // qDebug() << "Attempting to add a null link";
        return;
    }
     qDebug() << "m_links.size() =" << m_links.size();
     qDebug() << "Adding link" << link->m_id;
    if (std::find(m_links.begin(), m_links.end(), link) == m_links.end()) {
        try {
            m_links.push_back(link);
            // qDebug() << "Link added successfully. New m_links size:" << m_links.size();
        } catch (const std::exception& e) {
            qDebug() << "Exception caught while adding link:" << e.what();
        } catch (...) {
            qDebug() << "Unknown exception caught while adding link";
        }
        // qDebug() << "m_links.size() =" << m_links.size();
    }
}

void Node::removeLink(Link* link) {
    qDebug() << "Node::removeLink" << link->m_id << " from node" << m_id << "previous m_links.size() = " << m_links.size();
    auto it = std::find(m_links.begin(), m_links.end(), link);
    if (it != m_links.end()) {
        m_links.erase(it);
    }
    qDebug() << "new m_links.size() =" << m_links.size();
}

void Node::removeLinkById(int link_id) {
    auto it = std::find_if(m_links.begin(), m_links.end(),
                           [link_id](const Link* link) { qDebug() << "comparing" << link->m_id << "with" << link_id; return link->m_id == link_id; });
    if (it != m_links.end()) {
        qDebug() << "Removing link with id" << link_id;
        m_links.erase(it);
        qDebug() << "m_links.size() =" << m_links.size();
    }
}

void Node::setPosition(float x, float y) {
    setPos(x, y);
    updateLinks();
}

void Node::setPosition(QPointF& position) {
    setPosition(position.x(), position.y());
}

int Node::getGrade() const {
    return m_links.size();
}

std::vector<Link*> Node::getLinks() const {
    return m_links;
}

void Node::updateLinks() {
    for (Link* link : m_links) {
        link->trackNodes();
    }
}

bool Node::shouldReactToHover() const {
    Editor* editor = Editor::instance();
    for (const Link* link : m_links) {
        if (link->m_side == editor->m_currentSide || link->m_side == LinkSide::WIP) {
            return true;
        }
    }
    return false;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Link* link : m_links) {
            link->trackNodes();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

void Node::enable() {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
}

void Node::disable() {
    setFlags(QGraphicsItem::GraphicsItemFlags(0));
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (typeid(*this).name() == typeid(Node).name()) {
        if (shouldReactToHover()) {
            enable();
        } else {
            disable();
            return;
        }
    }

    if (m_showOnHover) {
        setOpacity(1);
        Editor::instance()->showStatusMessage(QString("Node %1").arg(m_id));
    }

    if (Pad* pad = dynamic_cast<Pad*>(this)) { // we can't directly cast as we'd do in C
        Editor::instance()->showStatusMessage(pad->m_name);
    }
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if (m_showOnHover) {
        setOpacity(0.001);
        Editor::instance()->showStatusMessage("");
    }

    if (typeid(*this).name() == typeid(Pad).name()) {
        Editor::instance()->showStatusMessage("");
    }

    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

QPainterPath Node::shape() const {
    QPainterPath path = QGraphicsEllipseItem::shape();
    path.addEllipse(QRectF(-10, -10, 20, 20));
    return path;
}

void Node::setSide(LinkSide side) {
    m_side = side;
    setParentItem(Editor::instance()->m_layers[side]);
}

void Node::refresh() {

    std::vector<LinkSide> sides;
    
    // get the list of link sides connected to this node
    for (const Link* link : m_links) {
        sides.push_back(link->m_side);
    }

    // if it's type Node (not Pad or any derivative), if it connects multiple sides, disable hover effect and show always
    if (typeid(*this) == typeid(Node)) {
        if (std::adjacent_find(sides.begin(), sides.end(), std::not_equal_to<>()) != sides.end()) {
            m_showOnHover = false;
            setOpacity(1);
        } else {
            m_showOnHover = true;
            setOpacity(0.01);
        }
    }
}

void Node::setColor(Color color) {
    int size = m_size.has_value() ? m_size.value() : Config::instance()->m_padSize;
    setRect(-size/2, -size/2, size, size);
    setPen(QPen(QColor(Config::instance()->color(color)), 0));
    setBrush(QBrush(QColor(Config::instance()->color(color))));
}

int Node::genNodeId() {
    return ++node_count;
}

int Node::getLastNodeId() {
    return node_count;
}


void Node::setNodeCount(int count) {
    if (count >= 0) {
        node_count = count;
    } else {
        throw std::invalid_argument("node_count must be a non-negative integer");
    }
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    m_drag_start_pos = pos();
    qDebug() << "click on node";
    QGraphicsEllipseItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_drag_start_pos != QPointF()) {
        QPointF drag_end_pos = pos();
        dragFinished(m_drag_start_pos, drag_end_pos);
        m_drag_start_pos = QPointF();
    }
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void Node::dragFinished(const QPointF& startPos, const QPointF& endPos) {
    if (startPos != endPos) {
        MoveNodeMeta meta{m_id, startPos, endPos};
        MoveNode* action = new MoveNode(meta);
        Editor::instance()->m_undoStack.push(action);
    }
}
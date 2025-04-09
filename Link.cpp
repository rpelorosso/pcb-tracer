#include "Link.h"
#include "Node.h"
#include "Editor.h"
#include "Config.h"

int TrackGraph::count = 0;
int Link::link_count = 0;

Link::Link(int id) : QGraphicsLineItem(), m_id(id), m_graphId(-1), m_my_from_node(nullptr), m_my_to_node(nullptr), m_side(LinkSide::FRONT) {
    Editor* editor = Editor::instance();

    m_text_item = new QGraphicsSimpleTextItem(QString::number(m_graphId), this);
    m_text_item->setBrush(QColor("#FFFFFF"));
    m_text_item->setZValue(1);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    m_text_item->setFont(font);
    m_text_item->setVisible(false);
    editor->scene()->addItem(m_text_item); 
    updateTextPosition();
} 

Link::~Link() {
    delete m_text_item;
}

void Link::updateTextPosition() {
    QLineF line = this->line();
    qreal line_length = line.length();
    qreal text_width = m_text_item->boundingRect().width();
    qreal text_x = line.pointAt(0.5).x() - text_width / 2;
    qreal text_y = line.pointAt(0.5).y() - m_text_item->boundingRect().height() / 2;
    m_text_item->setPos(text_x, text_y);
    m_text_item->setOpacity(0);
}

void Link::setGraphId(int graphId) {
    m_graphId = graphId;
    m_text_item->setText(QString::number(graphId));
}

void Link::setFromNode(Node* node) {
    m_my_from_node = node;
    m_my_from_node->addLink(this);
    trackNodes();
}

void Link::setToNode(Node* node) {
    m_my_to_node = node;
    m_my_to_node->addLink(this);
    trackNodes();
}

Node* Link::fromNode() const {
    return m_my_from_node;
}

Node* Link::toNode() const {
    return m_my_to_node;
}

void Link::setSide(LinkSide side) {
    m_side = side;
    qDebug() << "Link::setSide: " << LinkSideUtils::toString(side);
    setParentItem(Editor::instance()->m_layers[side]);
    trackNodes();
    refresh();
}

void Link::remove() {
    if (m_my_from_node) {
        m_my_from_node->removeLinkById(m_id);
    }
    if (m_my_to_node) {
        m_my_to_node->removeLinkById(m_id);
    }
    Editor::instance()->getScene()->removeItem(this);
    Editor::instance()->getScene()->removeItem(m_text_item);
}

void Link::setColor(Color color) {
    int width = m_width.has_value() ? m_width.value() : Config::instance()->m_linkWidth;   
    QPen pen(QColor(Config::instance()->color(color)), width);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
}

QColor Link::color() const {
    return pen().color();
}

void Link::trackNodes() {
    if (m_my_from_node && m_my_to_node) {
        setLine(QLineF(m_my_from_node->pos(), m_my_to_node->pos()));
        //qDebug() << "trackNodes: from=" << m_my_from_node->pos() << ", to=" << m_my_to_node->pos();
        updateTextPosition();
    }
}

void Link::refresh() {
    //qDebug() << "Link::refresh, tiene side: " << LinkSideUtils::toString(m_side) << "usa color: " << Config::instance()->color(m_side);
    setColor(ColorUtils::fromLinkSide(m_side));
    if (m_my_from_node) {
        m_my_from_node->refresh();
    }
    if (m_my_to_node) {
        m_my_to_node->refresh();
    }
}

void Link::setHighlighted(bool is_highlighted) {
    if (is_highlighted) {
        setColor(Color::HIGHLIGHTED);
    } else {
        refresh();
    }
}

void Link::notifyLinkChanges() {
    if (m_my_from_node) {
        m_my_from_node->notifyLinkChanges();
    }
    if (m_my_to_node) {
        m_my_to_node->notifyLinkChanges();
    }
}

int Link::genLinkId() {
    link_count++;
    return link_count;
}

int Link::getLastLinkId() {
    return link_count;
}

void Link::setLinkCount(int count) {
    if (count >= 0) {
        link_count = count;
    } else {
        throw std::invalid_argument("link_count must be a non-negative integer");
    }
}

void Link::updateTextItem(const QString& text) {
    qDebug() << "Link::updateTextItem: " << text;
}

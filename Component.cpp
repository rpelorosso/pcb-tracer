#include <QGraphicsScene>
#include <QPainter>
#include "Component.h"

int Component::s_componentCount = 0;

PhantomPad::PhantomPad(const QString& name, const QPointF& position): Node(-1) {
    m_name = name;
    setPos(position);
    setSize(Config::instance()->m_padSize);
    setColor(Color::PAD);
    setFlags(QGraphicsItem::ItemIsSelectable);
    setOpacity(1);
    m_showOnHover = false;
}

Pad::Pad(const QString& name, int id, const QPointF& position, int number) : Node(id), m_id(id), m_name(name), m_number(number) {
    m_componentId = -1;
    setPos(position);
    setFlags(QGraphicsItem::ItemIsSelectable);
    m_showOnHover = false;
    setOpacity(1);
    setSize(Config::instance()->m_padSize);
    setColor(Color::PAD);
}

Component::Component(const QString& name, int id) : m_name(name), m_id(id) {}

int Component::numberOfPads() const {
    return m_pads.size();
}

void Component::addPad(Pad* pad) {
    pad->m_componentId = m_id;
    m_pads.push_back(pad);
}

void Component::addToScene(QGraphicsScene* scene) {
    scene->addItem(this);
    for (size_t i = 0; i < m_pads.size(); ++i) {
        scene->addItem(m_pads[i]);
        qDebug() << "Added pad" << m_pads[i]->m_id << "to component" << m_id;
        m_pads[i]->setSide(LinkSide::NODE);
    }
}

QRectF Component::boundingRect() const {
    // Implement bounding rect calculation based on pads' positions
    // This is a placeholder implementation
    return QRectF();
}

void Component::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    // Implement painting logic for the component
    // This is a placeholder implementation
}

void Component::remove(QGraphicsScene* scene) {
    for (auto pad : m_pads) {
        scene->removeItem(pad);
        //delete pad;
    }
    //m_pads.clear();
    scene->removeItem(this);
}

int Component::genComponentId() {
    return ++s_componentCount;
}

int Component::getLastComponentId() {
    return s_componentCount;
}


void Component::setComponentCount(int count) {
    if (count >= 0) {
        s_componentCount = count;
    } else {
        throw std::invalid_argument("component_count must be a non-negative integer");
    }
}
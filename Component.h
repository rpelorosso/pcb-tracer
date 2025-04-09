#pragma once

#include <QGraphicsItem>
#include <QPointF>
#include <QString>
#include <vector>
#include "Node.h"
#include "enums.h"

class PhantomPad : public Node {
public:
    QString m_name;
    PhantomPad(const QString& name, const QPointF& position);
};

class Pad : public Node {
public:
    Pad(const QString& name, int id, const QPointF& position, int number);
    int m_componentId;
    int m_id;
    QString m_name;
    int m_number;
};

class Component : public QGraphicsItem {
public:
    Component(const QString& name, int id);

    int numberOfPads() const;
    QString toString() const;
    void addPad(Pad* pad);
    void addToScene(QGraphicsScene* scene);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void remove(QGraphicsScene* scene);

    static int genComponentId();
    static void setComponentCount(int count);
    static int getLastComponentId();
    int m_id;
    std::vector<Pad*> m_pads;
    QString m_name;

private:
    static int s_componentCount;
};
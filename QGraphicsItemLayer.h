#ifndef QGRAPHICSITEMLAYER_H
#define QGRAPHICSITEMLAYER_H

#include <QGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QPainterPath>

class QGraphicsItemLayer :  public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    QGraphicsItemLayer(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QPainterPath shape() const override;
    void setOpacity(qreal opacity);

private:
    QGraphicsOpacityEffect* m_opacityEffect;
};

#endif // QGRAPHICSITEMLAYER_H
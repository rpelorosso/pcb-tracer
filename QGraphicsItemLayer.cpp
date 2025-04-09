#include "QGraphicsItemLayer.h"
#include "Editor.h"

QGraphicsItemLayer::QGraphicsItemLayer(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1);
    setGraphicsEffect(m_opacityEffect);
}

QRectF QGraphicsItemLayer::boundingRect() const
{
    return QRectF(0, 0, 0, 0);
}

void QGraphicsItemLayer::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Empty implementation
}

QPainterPath QGraphicsItemLayer::shape() const
{
    return QPainterPath(); // Return an empty path, no shape to capture events
}

void QGraphicsItemLayer::setOpacity(qreal opacity)
{
    m_opacityEffect->setOpacity(opacity);
}
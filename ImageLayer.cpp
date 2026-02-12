#include "ImageLayer.h"
#include <QPixmap>
#include <QDebug>
#include <QTransform>
#include <QPolygonF>

ImageLayer::ImageLayer(int id) : QGraphicsPixmapItem(), m_id(id) {
    setZValue(-1);
    setPos(0, 0);
    setOpacity(1);
}

bool ImageLayer::loadImage(const QString& imagePath) {
    m_imagePath = imagePath;
    QPixmap pixmap;
    if (!pixmap.load(imagePath)) {
        qDebug() << "error";
    }
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
        initCornersFromPixmap();
        return true;
    } else {
        qDebug() << "Failed to load image:" << imagePath;
        return false;
    }
}

void ImageLayer::initCornersFromPixmap() {
    QRectF r = pixmap().rect().toRectF();
    m_corners[0] = r.topLeft();
    m_corners[1] = r.topRight();
    m_corners[2] = r.bottomRight();
    m_corners[3] = r.bottomLeft();
}

void ImageLayer::applyPerspectiveTransform() {
    QRectF r = pixmap().rect().toRectF();
    QPolygonF src;
    src << r.topLeft() << r.topRight() << r.bottomRight() << r.bottomLeft();
    QPolygonF dst;
    dst << m_corners[0] << m_corners[1] << m_corners[2] << m_corners[3];

    QTransform t;
    if (QTransform::quadToQuad(src, dst, t)) {
        setTransform(t);
        m_hasPerspectiveTransform = true;
    }
}

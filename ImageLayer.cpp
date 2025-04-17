#include "ImageLayer.h"
#include <QPixmap>
#include <QDebug>

/*
ImageLayer::ImageLayer(int id) : QGraphicsPixmapItem(), m_id(id) {
    setZValue(-1);
    setPos(0, 0);
    setOpacity(1);
}
 
bool ImageLayer::loadImage(const QString& imagePath) {
    m_imagePath = imagePath;
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
        return true;
    } else {
        std::cout << "Failed to load image: " << imagePath.toStdString() << std::endl;
        return false;
    }
}*/

ImageLayer::ImageLayer(int id) : QGraphicsPixmapItem(), m_id(id) {
    setZValue(-1);
    setPos(0, 0);  // Position image at the top-left corner of the view
    setOpacity(1);  // Set the image's opacity to 100%
}

bool ImageLayer::loadImage(const QString& imagePath) {
    m_imagePath = imagePath;
    QPixmap pixmap;
    if (!pixmap.load(imagePath)) {
        qDebug() << "error";
    }
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
        return true;
    } else {
        qDebug() << "Failed to load image:" << imagePath;
        return false;
    }
}

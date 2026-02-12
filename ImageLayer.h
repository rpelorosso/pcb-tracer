#ifndef IMAGELAYER_H
#define IMAGELAYER_H

#include <QGraphicsPixmapItem>
#include <QString>
#include <QPointF>
#include <array>

class ImageLayer : public QGraphicsPixmapItem {
public:
    explicit ImageLayer(int id);
    bool loadImage(const QString& imagePath);
    void initCornersFromPixmap();
    void applyPerspectiveTransform();

    int m_id;
    QString m_imagePath;
    std::array<QPointF, 4> m_corners;
    bool m_hasPerspectiveTransform = false;
};

#endif // IMAGELAYER_H

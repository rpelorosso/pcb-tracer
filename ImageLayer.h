#ifndef IMAGELAYER_H
#define IMAGELAYER_H

#include <QGraphicsPixmapItem>
#include <QString>

class ImageLayer : public QGraphicsPixmapItem {
public:
    explicit ImageLayer(int id);
    bool loadImage(const QString& imagePath);   
    int m_id;
    QString m_imagePath;

};

#endif // IMAGELAYER_H
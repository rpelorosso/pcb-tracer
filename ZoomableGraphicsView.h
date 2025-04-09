#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class ZoomableGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    ZoomableGraphicsView(QWidget* parent = nullptr);

    void flipHorizontal();
    void flipVertical();

    template<typename T>
    T* findItemByIdAndClass(const QString& itemId) const;

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void flipTransform(qreal flipX, qreal flipY);

    const qreal scaleFactor = 1.15;
    bool isPanning = false;
    QPoint panStartPoint;
};

template<typename T>
T* ZoomableGraphicsView::findItemByIdAndClass(const QString& itemId) const
{
    for (QGraphicsItem* item : scene()->items()) {
        T* castedItem = dynamic_cast<T*>(item);
        if (castedItem && castedItem->id() == itemId) {
            return castedItem;
        }
    }
    return nullptr;
}

#endif // ZOOMABLEGRAPHICSVIEW_H
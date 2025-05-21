#include "ZoomableGraphicsView.h"
#include <QScrollBar>
#include <QTransform>
#include <QDebug>
#ifdef USE_OPENGL
#include <QOpenGLWidget>
#endif
ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    #ifdef USE_OPENGL
    QOpenGLWidget *glWidget = new QOpenGLWidget(this);
    setViewport(glWidget);

    QSurfaceFormat format;
    format.setSamples(16);  // You can adjust this value (2, 4, 8, 16) for quality vs performance
    glWidget->setFormat(format);
    #endif
 
    // Set rendering hints for better performance
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

}

void ZoomableGraphicsView::flipHorizontal()
{
    flipTransform(-1, 1);
}

void ZoomableGraphicsView::flipVertical()
{
    flipTransform(1, -1);
}

void ZoomableGraphicsView::flipTransform(qreal flipX, qreal flipY)
{
    QTransform currentTransform = transform();

    qreal currentScaleX = currentTransform.m11();
    qreal currentScaleY = currentTransform.m22();
    qreal currentDx = currentTransform.dx();
    qreal currentDy = currentTransform.dy();

    QTransform newTransform;
    newTransform.scale(currentScaleX * flipX, currentScaleY * flipY);
    newTransform.translate(currentDx, currentDy);

    if (flipX == -1) {
        newTransform.translate(viewport()->width(), 0);
    }
    if (flipY == -1) {
        newTransform.translate(0, viewport()->height());
    }

    setTransform(newTransform);
}

void ZoomableGraphicsView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
            qDebug() << "Zooming in";
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void ZoomableGraphicsView::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "mouse press event: ";
    if (event->button() == Qt::MiddleButton) {
        qDebug() << "button";
        isPanning = true;
        panStartPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ZoomableGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        isPanning = false;
        setCursor(Qt::CrossCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ZoomableGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (isPanning) {
        QPoint delta = event->pos() - panStartPoint;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        panStartPoint = event->pos();
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

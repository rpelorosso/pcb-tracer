#include <QGraphicsScene>
#include <QPixmap>
#include <iostream>
#include "GuideTool.h"
#include "ImageLayer.h"
#include "Editor.h"


GuideTool::GuideTool() {}

GuideTool::~GuideTool() {
    // get all items and remove the ones of type ImageLayer
    auto items = Editor::instance()->scene()->items();
    for (auto item : items) {
        if (dynamic_cast<ImageLayer*>(item)) {
            Editor::instance()->scene()->removeItem(item);
            delete item;
        }
    }
}

void GuideTool::enterMode() {
    // no need for this now
}

void GuideTool::onMousePress(QMouseEvent* event) {
}

void GuideTool::onKeyPress(QKeyEvent* event) {
}

void GuideTool::onKeyRelease(QKeyEvent* event) {
}

void GuideTool::onMouseMove(QMouseEvent* event) {
}

void GuideTool::setLayerOpacity(LinkSide side, qreal alpha) {
    int layerId = static_cast<int>(side);
    ImageLayer* layer = dynamic_cast<ImageLayer*>(Editor::instance()->findItemByIdAndClass<ImageLayer>(layerId));
    if (layer != nullptr) {
        layer->setOpacity(alpha);
    }
}

void GuideTool::clear() {
    // get all items and remove the ones of type ImageLayer
    auto items = Editor::instance()->scene()->items();
    for (auto item : items) {
        if (dynamic_cast<ImageLayer*>(item)) {
            Editor::instance()->scene()->removeItem(item);
        }
    }
}

void GuideTool::setImageLayer(LinkSide side, const QString& imagePath) {
    int layerId = static_cast<int>(side);
    ImageLayer* layer = dynamic_cast<ImageLayer*>(Editor::instance()->findItemByIdAndClass<ImageLayer>(layerId));
    if (layer == nullptr) {
        layer = new ImageLayer(static_cast<int>(side));
        switch (side) {
            case LinkSide::FRONT:
                layer->setZValue(-4);
                break;
            case LinkSide::BACK:
                layer->setZValue(-3);
                break;
            case LinkSide::WIP:
                layer->setZValue(-2);
                break;
            default:
                layer->setZValue(-1);
                break;
        }

        layer->loadImage(imagePath);

        Editor::instance()->scene()->addItem(layer);
    }
    std::cout << "layer image " << layer << std::endl;
    layer->loadImage(imagePath);
}
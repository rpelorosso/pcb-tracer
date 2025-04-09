#pragma once

#include <QGraphicsView>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QUndoCommand>
#include <QUndoStack>
#include <tuple>
#include <vector>
#include <QString>
#include "../Component.h"

struct AddComponentMeta {
    QString m_name;
    std::vector<Pad*> m_pads;
};


class AddComponent : public QUndoCommand {
public:
    AddComponent(const AddComponentMeta& meta);

    void undo() override;
    void redo() override;
  
private:
    AddComponentMeta m_meta;
    QGraphicsScene* m_scene;
    int m_componentId;
    Component* m_component;
};
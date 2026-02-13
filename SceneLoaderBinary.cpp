#include "SceneLoaderBinary.h"
#include "Editor.h"
#include "Component.h"
#include "Link.h"
#include "Node.h"
#include "ImageLayer.h"
#include "CommunicationHub.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include "NotesTool.h"

bool SceneLoaderBinary::loadSceneFromBinary(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_15);

        // Read and check the magic number
        char magic[7];
        in.readRawData(magic, 6);
        magic[6] = '\0';  // Null-terminate the string
        if (strcmp(magic, "PCBTRC") != 0) {
            qDebug() << "Invalid file format";
            return false;
        }

        // Read and check the version
        qint32 version;
        in >> version;
        if (version < 1 || version > 3) {
            qDebug() << "Unsupported file version:" << version;
            return false;
        }

        Editor* editor = Editor::instance();
        //editor->clean(); // Clear the current scene

        // Create a temporary map to store nodes
        QMap<int, Node*> nodeMap;
        // Read scene elements
        while (!in.atEnd()) {
            quint8 elementType;
            in >> elementType;

            switch (static_cast<SceneElementType>(elementType)) 
            {
                case SceneElementType::Config:
                    readConfigFromBinary(in, version);
                    break;
                case SceneElementType::Component:
                    readComponentFromBinary(in, nodeMap);
                    break;
                case SceneElementType::Link:
                    readLinkFromBinary(in, nodeMap);
                    break;
                case SceneElementType::Node:
                    readNodeFromBinary(in, nodeMap);
                    break;
                case SceneElementType::ImageLayer:
                    readImageLayerFromBinary(in);
                    break;
                case SceneElementType::TextNote:
                    readTextNoteFromBinary(in);
                    break;
                case SceneElementType::LastIds:
                    readLastIds(in);
                    break;
                default:
                    qDebug() << "Unknown element type:" << elementType;
                    return false;
            }
        }

        file.close();

        // Notify changes for all nodes
        for (auto node : nodeMap) {
            if (node && !dynamic_cast<Pad*>(node)) {
                node->notifyLinkChanges();
            }
        }

        qDebug() << "Scene loaded from" << filename;
        editor->showStatusMessage("Scene loaded successfully");


        // Clear the temporary node map
        nodeMap.clear();
        return true;
    } else {
        qDebug() << "Failed to open file for reading:" << filename;
        return false;
    }
}

bool SceneLoaderBinary::saveSceneToBinary(const QString& filename) {
    QString actualFilename = filename;
    if (!actualFilename.toLower().endsWith(".pcb")) {
        actualFilename += ".pcb";
    }

    QFile file(actualFilename);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_15);

        // Write a magic number for file format identification
        out.writeRawData("PCBTRC", 6);

        // Write the version number
        out << (qint32)3;  // version 3 - adds pad color to config

        writeLastIds(out);

        QGraphicsScene* scene = Editor::instance()->scene();

        // Write configuration
        out << (quint8)SceneElementType::Config;
        writeConfigToBinary(out);

        // Write nodes first
        for (QGraphicsItem* item : scene->items()) {
            if (auto node = dynamic_cast<Node*>(item)) {
                if (!dynamic_cast<Pad*>(node)) {
                    out << (quint8)SceneElementType::Node;
                    writeNodeToBinary(out, node);
                }
            }
        }

        // Write components (including pads)
        for (QGraphicsItem* item : scene->items()) {
            if (auto component = dynamic_cast<Component*>(item)) {
                out << (quint8)SceneElementType::Component;
                writeComponentToBinary(out, component);
            }
        }

        // Write links
        for (QGraphicsItem* item : scene->items()) {
            if (auto link = dynamic_cast<Link*>(item)) {
                out << (quint8)SceneElementType::Link;
                writeLinkToBinary(out, link);
            }
        }

        // Write image layers
        for (QGraphicsItem* item : scene->items()) {
            if (auto imageLayer = dynamic_cast<ImageLayer*>(item)) {
                out << (quint8)SceneElementType::ImageLayer;
                writeImageLayerToBinary(out, imageLayer);
            }
        }

        // Write image layers
        for (QGraphicsItem* item : scene->items()) {
            if (auto textNote = dynamic_cast<TextNote*>(item)) {
                out << (quint8)SceneElementType::TextNote;
                writeTextNoteToBinary(out, textNote);
            }
        }

        file.close();
        qDebug() << "Scene data saved to" << actualFilename;
        return true;
    } else {
        qDebug() << "Failed to save scene data to" << actualFilename;
        return false;
    }
}

void SceneLoaderBinary::writeComponentToBinary(QDataStream& out, Component* component) {
    out << component->m_id << component->m_name
        << component->pos().x() << component->pos().y();

    out << (quint32)component->m_pads.size();
    for (const auto& pad : component->m_pads) {
        writePadToBinary(out, pad);
    }
}

void SceneLoaderBinary::writePadToBinary(QDataStream& out, Pad* pad) {
    out << pad->m_id << pad->pos().x() << pad->pos().y()
        << pad->m_number << pad->m_name;        
}

void SceneLoaderBinary::writeLinkToBinary(QDataStream& out, Link* link) {
    out << link->m_id << link->fromNode()->m_id << link->toNode()->m_id
        << link->m_graphId << (quint8)link->m_side;

    if (link->m_width.has_value()) {
        out << (quint8)1;  // Indicate that m_width has a value
        out << (qreal)link->m_width.value();
    } else {
        out << (quint8)0;  // Indicate that m_width does not have a value
    }
}

void SceneLoaderBinary::writeNodeToBinary(QDataStream& out, Node* node) {
    out << node->m_id << node->pos().x() << node->pos().y();
}

void SceneLoaderBinary::writeImageLayerToBinary(QDataStream& out, ImageLayer* imageLayer) {
    out << (qint32)imageLayer->m_id << imageLayer->m_imagePath
        << imageLayer->pos().x() << imageLayer->pos().y()
        << imageLayer->opacity();
}

void SceneLoaderBinary::writeTextNoteToBinary(QDataStream& out, TextNote* textNote) {
    out << textNote->m_id 
        << textNote->rect().x() 
        << textNote->rect().y() 
        << textNote->rect().width() 
        << textNote->rect().height()
        << textNote->m_text;
}

void SceneLoaderBinary::writeLastIds(QDataStream& out) {
    out << (quint8)SceneElementType::LastIds;
    out << Component::getLastComponentId()
        << Link::getLastLinkId()
        << NotesTool::getLastNoteId()
        << Node::getLastNodeId();
}


void SceneLoaderBinary::writeConfigToBinary(QDataStream& out) {
    out << Config::instance()->color(Color::FRONT);
    out << Config::instance()->color(Color::BACK);
    out << Config::instance()->color(Color::HIGHLIGHTED);
    out << Config::instance()->color(Color::NODE);
    out << Config::instance()->color(Color::NOTES);
    out << Config::instance()->color(Color::WIP);
    out << Config::instance()->m_linkWidth;
    out << Config::instance()->m_padSize;
    out << Config::instance()->m_nodeSize;
    out << Config::instance()->color(Color::PAD);
}


void SceneLoaderBinary::readConfigFromBinary(QDataStream& in, qint32 version) {
    Config::instance()->readConfigFromBinary(in);
    if (version >= 2) {
        in >> Config::instance()->m_nodeSize;
    }
    if (version >= 3) {
        in >> Config::instance()->m_colors[Color::PAD];
    }
}

void SceneLoaderBinary::readNodeFromBinary(QDataStream& in, QMap<int, Node*>& nodeMap) {
    int id;
    qreal x, y;
    in >> id >> x >> y;
    qDebug() << "Reading node with ID" << id << "at" << x << "," << y;
    Node* node = new Node(id);
    node->setPos(x, y);
    node->setSide(LinkSide::NODE); // This adds it to the scene
    nodeMap[id] = node;
}


Pad* SceneLoaderBinary::readPadFromBinary(QDataStream& in) {
    int id, number;
    qreal x, y;
    QString name;
    in >> id >> x >> y >> number >> name;
    qDebug() << "Reading pad with ID" << id << "name" << name << "at" << x << "," << y << "number" << number;
    // Create a new Pad object
    Pad* pad = new Pad(name, id, QPointF(x, y), number);
    return pad;
}
void SceneLoaderBinary::readComponentFromBinary(QDataStream& in, QMap<int, Node*>& nodeMap) {
    int id;
    QString name;
    qreal x, y;
    in >> id >> name >> x >> y;

    Component* component = new Component(name, id);
    component->setPos(x, y);

    quint32 padCount;
    in >> padCount;
    for (quint32 i = 0; i < padCount; ++i) {
        Pad* pad = readPadFromBinary(in);
        component->addPad(pad);
        nodeMap[pad->m_id] = pad;
    }

    component->addToScene(Editor::instance()->scene());
    CommunicationHub::instance().publish(HubEvent::COMPONENT_CREATED, component);
}




void SceneLoaderBinary::readLinkFromBinary(QDataStream& in, const QMap<int, Node*>& nodeMap) {
    int id, fromNodeId, toNodeId, graphId;
    quint8 side, hasWidth;
    qreal width;

    in >> id >> fromNodeId >> toNodeId >> graphId >> side;

    // Read the width
    in >> hasWidth;
    if (hasWidth) {
        qint32 width;
        in >> width;
    }    

    qDebug() << "Reading link with ID" << id << "from node" << fromNodeId << "to node" << toNodeId << "with width" << width;

    Node* fromNode = nodeMap.value(fromNodeId);
    Node* toNode = nodeMap.value(toNodeId);

    if (fromNode && toNode) {
        Link* link = new Link(id);
        link->setFromNode(fromNode);
        link->setToNode(toNode);
        link->setGraphId(graphId);
        if(hasWidth) link->m_width = width;
        link->setSide(static_cast<LinkSide>(side));
        // Set width if needed
        link->refresh();
    } else {
        qDebug() << "Failed to create link: node not found";
    }
}

void SceneLoaderBinary::readImageLayerFromBinary(QDataStream& in) {
    qint32 id;
    QString imagePath;
    qreal x, y, opacity;
    in >> id >> imagePath >> x >> y >> opacity;
    qDebug() << "Reading image layer with ID" << id << "at" << x << "," << y << "opacity" << opacity << "image path" << imagePath;
    Editor::instance()->m_guideTool->setImageLayer(static_cast<LinkSide>(id), imagePath);
}

void SceneLoaderBinary::readTextNoteFromBinary(QDataStream& in) {
    int id;
    qreal x, y, width, height;
    QString text;

    in >> id >> x >> y >> width >> height >> text;

    qDebug() << "Reading TextNote with ID" << id << "at" << x << "," << y << "with size" << width << "x" << height;

    QRectF rect(x, y, width, height);
    TextNote* textNote = new TextNote(rect, Config::instance()->color(Color::NOTES));
    textNote->m_id = id;
    textNote->setText(text);

    Editor::instance()->scene()->addItem(textNote);
    textNote->setParentItem(Editor::instance()->m_layers[LinkSide::NOTES]);

    CommunicationHub::instance().publish(HubEvent::NOTE_CREATED, textNote);
}

void SceneLoaderBinary::readLastIds(QDataStream& in) {
    int component, link, note, node;
    in >> component >> link >> note >> node;
    Component::setComponentCount(component);
    Link::setLinkCount(link);
    NotesTool::setNoteCount(note);
    Node::setNodeCount(node);
}


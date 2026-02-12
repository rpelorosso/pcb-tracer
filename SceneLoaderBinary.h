#ifndef SCENELOADERBINARY_H
#define SCENELOADERBINARY_H

#include <QString>
#include <QJsonObject>
#include <QDataStream>
#include <QMap>

class Component;
class Link;
class Node;
class Pad;
class ImageLayer;
class TextNote;

enum class SceneElementType : quint8 {
    Component = 1,
    Link = 2,
    Pad = 3,
    Node = 4,
    ImageLayer = 5,
    TextNote = 6,
    LastIds = 7,
    Config = 8
};

class SceneLoaderBinary {
public:
    static bool loadSceneFromBinary(const QString& filename);
    static bool saveSceneToBinary(const QString& filename);

private:
    static void writeComponentToBinary(QDataStream& out, Component* component);
    static void writePadToBinary(QDataStream& out, Pad* pad);
    static void writeLinkToBinary(QDataStream& out, Link* link);
    static void writeNodeToBinary(QDataStream& out, Node* node);
    static void writeTextNoteToBinary(QDataStream& out, TextNote* textNote);
    static void writeImageLayerToBinary(QDataStream& out, ImageLayer* imageLayer);
    static void writeLastIds(QDataStream& out);
    static void writeConfigToBinary(QDataStream& out);

    static void readConfigFromBinary(QDataStream& in);
    static void readComponentFromBinary(QDataStream& in, QMap<int, Node*>& nodeMap);
    static Pad* readPadFromBinary(QDataStream& in);
    static void readLinkFromBinary(QDataStream& in, const QMap<int, Node*>& nodeMap);
    static void readNodeFromBinary(QDataStream& in, QMap<int, Node*>& nodeMap);
    static void readImageLayerFromBinary(QDataStream& in, qint32 version);
    static void readTextNoteFromBinary(QDataStream& in);
    static void readLastIds(QDataStream& in);

    SceneLoaderBinary() = delete;  // Prevent instantiation
    SceneLoaderBinary(const SceneLoaderBinary&) = delete;  // Prevent copy
    SceneLoaderBinary& operator=(const SceneLoaderBinary&) = delete;  // Prevent assignment
};

#endif // SCENELOADERBINARY_H
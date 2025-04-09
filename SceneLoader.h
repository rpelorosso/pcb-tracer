#ifndef SCENELOADER_H
#define SCENELOADER_H

#include <QString>
#include <QJsonObject>

class SceneLoader {
public:
    static bool loadSceneFromJson(const QString& filename);
    static bool saveSceneToJson(const QString& filename);
    static bool saveSceneToBinary(const QString& filename);
    static QVariantMap getQVariantMapSceneElements();
    static QJsonObject getSceneElements();
private:
    SceneLoader() = delete;  // Prevent instantiation
    SceneLoader(const SceneLoader&) = delete;  // Prevent copy
    SceneLoader& operator=(const SceneLoader&) = delete;  // Prevent assignment
};

#endif // SCENELOADER_H
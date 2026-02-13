#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QVariantMap>
#include <QMap>
#include "enums.h"

class Editor;
class Link;

class Config {
public:
    static Config* instance();
    QString color(LinkSide side) const;
    QString color(Color color) const;
    void apply();
    void updateFromConfigDialog(const QVariantMap& dialogConfig);
    void update(const QVariantMap& kwargs);
    QVariantMap toDict() const;
    void readConfigFromBinary(QDataStream& in);
    
    int m_linkWidth;
    int m_padSize;
    int m_nodeSize;

    QMap<Color, QString> m_colors;

private:
    Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config* m_instance;

};

#endif // CONFIG_H
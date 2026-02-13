#include "Config.h"
#include "Link.h"
#include "Node.h"
#include "Editor.h"
#include "Component.h"
#include <QDebug>

Config* Config::m_instance = nullptr;

Config* Config::instance() {
    if (!m_instance) {
        m_instance = new Config();
    }
    return m_instance;
}

Config::Config() {
    // Initialize default colors
    m_colors = {
        {Color::FRONT, "#FF0000"},
        {Color::BACK, "#00FF00"},
        {Color::WIP, "#00FFFF"},
        {Color::NOTES, "#FFFFFF"},
        {Color::HIGHLIGHTED, "#FFFFFF"},
        {Color::NODE, "#00FF00"},
        {Color::PAD, "#ff007f"}
    };

    // Initialize other default settings
    m_linkWidth = 6;
    m_padSize = 17;
    m_nodeSize = 13;
}

void Config::apply() {
    for (QGraphicsItem* item : Editor::instance()->scene()->items()) {
        if (auto* link = dynamic_cast<Link*>(item)) {
            link->m_width.reset();
            link->setColor(ColorUtils::fromLinkSide(link->m_side));
        } else if (auto* pad = dynamic_cast<Pad*>(item)) {
            pad->setSize(m_padSize);
            pad->setColor(Color::PAD);
        } else if (auto* node = dynamic_cast<Node*>(item)) {
            node->setSize(m_nodeSize);
            node->setColor(Color::NODE);
        }
    }
    // trigger re-rendering of color box
    Editor::instance()->setCurrentSide(Editor::instance()->m_currentSide);
}

void Config::updateFromConfigDialog(const QVariantMap& dialogConfig) {
    // Update colors
    QVariantMap colorMap = dialogConfig["colors"].toMap();
    for (auto it = colorMap.begin(); it != colorMap.end(); ++it) {
        Color color = ColorUtils::fromString(it.key());
        m_colors[color] = it.value().toString();
    }

    // Update other settings
    m_linkWidth = dialogConfig["link_width"].toInt();
    m_padSize = dialogConfig["pad_size"].toInt();
    if (dialogConfig.contains("node_size"))
        m_nodeSize = dialogConfig["node_size"].toInt();
}

void Config::readConfigFromBinary(QDataStream& in) {
    in  >> m_colors[Color::FRONT]
        >> m_colors[Color::BACK]
        >> m_colors[Color::HIGHLIGHTED]
        >> m_colors[Color::NODE]
        >> m_colors[Color::NOTES]
        >> m_colors[Color::WIP]
        >> m_linkWidth
        >> m_padSize;
}

QString Config::color(LinkSide side) const {
    return m_colors.value(ColorUtils::fromLinkSide(side), "#000000");
}

QString Config::color(Color color) const {
    return m_colors.value(color, "#000000");
}

QVariantMap Config::toDict() const {
    QVariantMap result;
    QVariantMap colorMap;
    for (auto it = m_colors.begin(); it != m_colors.end(); ++it) {
        colorMap[ColorUtils::toString(it.key())] = it.value();
    }
    result["colors"] = colorMap;
    result["link_width"] = m_linkWidth;
    result["pad_size"] = m_padSize;
    result["node_size"] = m_nodeSize;
    return result;
}
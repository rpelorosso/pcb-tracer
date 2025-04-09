#ifndef LINK_H
#define LINK_H

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QColor>
#include <QFont>
#include "Config.h"
#include "enums.h"

class Node;

class TrackGraph {
public:
    static int count;

    static int genTrackGraphId() {
        count++;
        return count;
    }

    static void setTrackGraphCount(int count) {
        if (count >= 0) {
            TrackGraph::count = count;
        } else {
            throw std::invalid_argument("count must be a non-negative integer");
        }
    }
};

class Link : public QGraphicsLineItem {
public:
    static int link_count;

    Link(int id);
    ~Link();

    void setGraphId(int graph_id);
    void setFromNode(Node* node);
    void setToNode(Node* node);
    Node* fromNode() const;
    Node* toNode() const;
    void setSide(LinkSide side);
    void remove();
    void setColor(Color color);
    QColor color() const;
    void trackNodes();
    void refresh();
    void setHighlighted(bool is_highlighted);
    void notifyLinkChanges();

    static int genLinkId();
    static int getLastLinkId();
    static void setLinkCount(int count);

    void updateTextItem(const QString& text);


    LinkSide m_side;
    int m_id;
    int m_graphId;
    std::optional<int> m_width;

private:
    void updateTextPosition();

    Node* m_my_from_node;
    Node* m_my_to_node;
    QGraphicsSimpleTextItem* m_text_item;
};

#endif // LINK_H
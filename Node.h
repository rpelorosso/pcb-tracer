#ifndef NODE_H
#define NODE_H

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <vector>
#include "Config.h"
#include "enums.h"
#include "CommunicationHub.h"

class Link;

class Node : public QGraphicsEllipseItem {
public:
    static int node_count;
    bool m_showOnHover;

    Node(int id);

    void notifyLinkChanges();
    void willBeDeleted();
    void addLink(Link* link);
    void removeLink(Link* link);
    void removeLinkById(int link_id);
    void setPosition(float x, float y);
    
    void setPosition(QPointF& position);
    int getGrade() const;
    std::vector<Link*> getLinks() const;
    void updateLinks();
    bool shouldReactToHover() const;
    void enable();
    void disable();
    void setSide(LinkSide side);
    void refresh();
    void setColor(Color color);
    static int genNodeId();
    static int getLastNodeId();
    static void setNodeCount(int count);
    int m_id;
    std::optional<int> m_size;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QPainterPath shape() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    std::vector<Link*> m_links;
    LinkSide m_side;
    QPointF m_drag_start_pos;

    void dragFinished(const QPointF& startPos, const QPointF& endPos);
};

#endif // NODE_H
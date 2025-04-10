#ifndef NODE_H
#define NODE_H

#include "types.h"
#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

class Node : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Node(QGraphicsItem* parent = nullptr);
    ~Node() override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    virtual void process() = 0;
    virtual std::string name() const = 0;
    virtual std::vector<Port> getPorts() const = 0;

    void addInputConnection(Node* sourceNode, int sourcePort, int destPort);
    void removeInputConnection(int port);
    std::vector<std::pair<Node*, int>> getInputConnections() const;

    void setOutputData(int portIndex, std::shared_ptr<void> data);
    std::shared_ptr<void> getInputData(int portIndex) const;

    NodeID id() const { return m_id; }

signals:
    void dataUpdated();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    NodeID m_id;
    std::vector<std::pair<Node*, int>> m_inputConnections;
    std::vector<std::shared_ptr<void>> m_outputData;
    QPointF m_oldPos;
};

#endif // NODE_H
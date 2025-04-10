#ifndef NODEGRAPH_H
#define NODEGRAPH_H

#include "Node.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QPointF>
#include <vector>

class NodeGraph : public QGraphicsScene {
    Q_OBJECT
public:
    explicit NodeGraph(QObject* parent = nullptr);
    ~NodeGraph() override;

    void addNode(Node* node);
    void removeNode(Node* node);
    void connectNodes(Node* sourceNode, int sourcePort, Node* destNode, int destPort);
    void disconnectNodes(Node* destNode, int destPort);
    void processGraph();

    std::vector<Node*> getNodes() const { return m_nodes; }

signals:
    void nodeAdded(Node* node);
    void nodeRemoved(Node* node);
    void connectionMade(Node* sourceNode, int sourcePort, Node* destNode, int destPort);
    void connectionRemoved(Node* destNode, int destPort);
    void graphProcessed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::vector<Node*> m_nodes;
    QGraphicsLineItem* m_tempConnection = nullptr;
    Node* m_connectionStartNode = nullptr;
    int m_connectionStartPort = -1;
};

#endif // NODEGRAPH_H
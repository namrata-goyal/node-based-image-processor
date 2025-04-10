#include "NodeGraph.h"
#include "Node.h"
#include <QGraphicsLineItem>
#include <QPen>

NodeGraph::NodeGraph(QObject* parent) : QGraphicsScene(parent) {}

NodeGraph::~NodeGraph() {
    for (auto node : m_nodes) {
        removeItem(node);
        delete node;
    }
}

void NodeGraph::addNode(Node* node) {
    m_nodes.push_back(node);
    addItem(node);
    connect(node, &Node::dataUpdated, this, [this]() { processGraph(); });
    emit nodeAdded(node);
}

void NodeGraph::removeNode(Node* node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        // Remove all connections to this node
        for (auto otherNode : m_nodes) {
            auto connections = otherNode->getInputConnections();
            for (size_t i = 0; i < connections.size(); ++i) {
                if (connections[i].first == node) {
                    otherNode->removeInputConnection(i);
                    emit connectionRemoved(otherNode, i);
                }
            }
        }

        m_nodes.erase(it);
        removeItem(node);
        emit nodeRemoved(node);
        delete node;
    }
}

void NodeGraph::connectNodes(Node* sourceNode, int sourcePort, Node* destNode, int destPort) {
    destNode->addInputConnection(sourceNode, sourcePort, destPort);
    emit connectionMade(sourceNode, sourcePort, destNode, destPort);
    processGraph();
}

void NodeGraph::disconnectNodes(Node* destNode, int destPort) {
    destNode->removeInputConnection(destPort);
    emit connectionRemoved(destNode, destPort);
    processGraph();
}

void NodeGraph::processGraph() {
    // Simple processing - just process all nodes in order
    // A more sophisticated implementation would topologically sort the graph
    for (auto node : m_nodes) {
        node->process();
    }
    emit graphProcessed();
}

void NodeGraph::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item && dynamic_cast<Node*>(item))) {
            Node* node = dynamic_cast<Node*>(item);
            QPointF pos = node->mapFromScene(event->scenePos());
            QRectF bounds = node->boundingRect();
            
            // Check if click was on a port
            auto ports = node->getPorts();
            for (size_t i = 0; i < ports.size(); ++i) {
                const auto& port = ports[i];
                QRectF portRect;
                if (port.type == PortType::Input) {
                    portRect = QRectF(5, 25 + i * 20, 10, 10);
                } else {
                    portRect = QRectF(bounds.width() - 15, 25 + i * 20, 10, 10);
                }
                
                if (portRect.contains(pos)) {
                    m_connectionStartNode = node;
                    m_connectionStartPort = i;
                    m_tempConnection = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos()));
                    m_tempConnection->setPen(QPen(Qt::black, 2));
                    addItem(m_tempConnection);
                    return;
                }
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void NodeGraph::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_tempConnection) {
        QLineF newLine(m_tempConnection->line().p1(), event->scenePos());
        m_tempConnection->setLine(newLine);
    } else {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void NodeGraph::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (m_tempConnection && event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item && dynamic_cast<Node*>(item))) {
            Node* endNode = dynamic_cast<Node*>(item);
            QPointF pos = endNode->mapFromScene(event->scenePos());
            QRectF bounds = endNode->boundingRect();
            
            auto ports = endNode->getPorts();
            for (size_t i = 0; i < ports.size(); ++i) {
                const auto& port = ports[i];
                QRectF portRect;
                if (port.type == PortType::Input) {
                    portRect = QRectF(5, 25 + i * 20, 10, 10);
                } else {
                    portRect = QRectF(bounds.width() - 15, 25 + i * 20, 10, 10);
                }
                
                if (portRect.contains(pos)) {
                    // Check if connection is valid
                    auto startPort = m_connectionStartNode->getPorts()[m_connectionStartPort];
                    auto endPort = port;
                    
                    if (startPort.type != endPort.type && 
                        startPort.dataType == endPort.dataType) {
                        if (startPort.type == PortType::Output) {
                            connectNodes(m_connectionStartNode, m_connectionStartPort, endNode, i);
                        } else {
                            connectNodes(endNode, i, m_connectionStartNode, m_connectionStartPort);
                        }
                    }
                    break;
                }
            }
        }
        
        removeItem(m_tempConnection);
        delete m_tempConnection;
        m_tempConnection = nullptr;
        m_connectionStartNode = nullptr;
        m_connectionStartPort = -1;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
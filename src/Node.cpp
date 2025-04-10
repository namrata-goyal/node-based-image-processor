#include "Node.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

static NodeID nextNodeID = 1;

Node::Node(QGraphicsItem* parent) : QGraphicsItem(parent), m_id(nextNodeID++) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Node::~Node() {}

QRectF Node::boundingRect() const {
    return QRectF(0, 0, 150, 100 + getPorts().size() * 20);
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(widget);

    // Draw node background
    QColor fillColor = isSelected() ? QColor(100, 100, 150) : QColor(80, 80, 120);
    painter->setBrush(fillColor);
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRoundedRect(boundingRect(), 5, 5);

    // Draw node title
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(QRectF(0, 0, boundingRect().width(), 20), Qt::AlignCenter, QString::fromStdString(name()));

    // Draw ports
    auto ports = getPorts();
    for (size_t i = 0; i < ports.size(); ++i) {
        const auto& port = ports[i];
        QRectF portRect(5, 25 + i * 20, 140, 20);

        if (port.type == PortType::Input) {
            painter->setBrush(Qt::darkGreen);
            painter->drawEllipse(portRect.left(), portRect.top() + 5, 10, 10);
            painter->drawText(portRect.adjusted(15, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, 
                            QString::fromStdString(port.name));
        } else {
            painter->setBrush(Qt::darkRed);
            painter->drawEllipse(portRect.right() - 10, portRect.top() + 5, 10, 10);
            painter->drawText(portRect.adjusted(0, 0, -15, 0), Qt::AlignRight | Qt::AlignVCenter, 
                            QString::fromStdString(port.name));
        }
    }
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    m_oldPos = pos();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (pos() != m_oldPos) {
        // Position changed - notify connections
        emit dataUpdated();
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void Node::addInputConnection(Node* sourceNode, int sourcePort, int destPort) {
    if (destPort < static_cast<int>(m_inputConnections.size())) {
        m_inputConnections[destPort] = {sourceNode, sourcePort};
    }
}

void Node::removeInputConnection(int port) {
    if (port < static_cast<int>(m_inputConnections.size())) {
        m_inputConnections[port] = {nullptr, -1};
    }
}

std::vector<std::pair<Node*, int>> Node::getInputConnections() const {
    return m_inputConnections;
}

void Node::setOutputData(int portIndex, std::shared_ptr<void> data) {
    if (portIndex >= 0 && portIndex < static_cast<int>(m_outputData.size())) {
        m_outputData[portIndex] = data;
    }
}

std::shared_ptr<void> Node::getInputData(int portIndex) const {
    if (portIndex >= 0 && portIndex < static_cast<int>(m_inputConnections.size())) {
        const auto& connection = m_inputConnections[portIndex];
        if (connection.first) {
            return connection.first->getOutputData(connection.second);
        }
    }
    return nullptr;
}
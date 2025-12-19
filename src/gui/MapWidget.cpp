#include "MapWidget.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QRandomGenerator>
#include <QBrush>
#include <QPen>
#include <cmath>

// Constants for layout
const int SCENE_WIDTH = 800;
const int SCENE_HEIGHT = 600;
const int GRID_SIZE = 50;

MapWidget::MapWidget(Simulator* simulator, QWidget* parent)
    : QGraphicsView(parent), m_simulator(simulator) {
    
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    setScene(m_scene);
    
    // Antialiasing for smoother drawing
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setBackgroundBrush(QBrush(QColor(30, 30, 30))); // Dark background
    
    setupScene();
    
    // Animation timer (30 FPS)
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &MapWidget::updateAnimation);
    m_animationTimer->start(33);
    
    // Connect to simulator signals if needed, or rely on MainWindow to call refresh
    // For now, MainWindow usually updates us on simulation steps
}

MapWidget::~MapWidget() {}

void MapWidget::setupScene() {
    drawGrid();
    
    // Initialize node positions
    int rows = SCENE_HEIGHT / 100;
    int cols = SCENE_WIDTH / 100;
    
    // Create random positions for 100 nodes
    for (int i = 0; i < 100; ++i) {
        // ... (existing logic for positions is fine, let's just use it)
        int r = (i / cols) % rows;
        int c = i % cols;
        int x = c * 100 + 50 + QRandomGenerator::global()->bounded(-30, 30);
        int y = r * 100 + 50 + QRandomGenerator::global()->bounded(-30, 30);
        QPointF pos(x, y);
        m_nodePositions[i] = pos;
        
        // Draw faint "Customer Node" if not a known warehouse (0,1,2 usually)
        if (i >= 3) {
            QGraphicsEllipseItem* house = m_scene->addEllipse(0, 0, 6, 6, Qt::NoPen, QBrush(QColor(60, 60, 60)));
            house->setPos(pos - QPointF(3, 3));
            house->setZValue(0);
        }
    }
}

void MapWidget::drawGrid() {
    QPen gridPen(QColor(60, 60, 60), 1, Qt::DotLine);
    for (int x = 0; x <= SCENE_WIDTH; x += GRID_SIZE) {
        m_scene->addLine(x, 0, x, SCENE_HEIGHT, gridPen);
    }
    for (int y = 0; y <= SCENE_HEIGHT; y += GRID_SIZE) {
        m_scene->addLine(0, y, SCENE_WIDTH, y, gridPen);
    }
}

QPointF MapWidget::getNodePosition(int nodeId) {
    if (!m_nodePositions.contains(nodeId)) {
        m_nodePositions[nodeId] = QPointF(SCENE_WIDTH/2, SCENE_HEIGHT/2);
    }
    return m_nodePositions[nodeId];
}

void MapWidget::refresh() {
    const auto& warehouses = m_simulator->getWarehouses();
    
    // 1. Draw Warehouses
    for (const auto& pair : warehouses) {
        int id = pair.first;
        const Warehouse& w = pair.second;
        
        if (!m_warehouseItems.contains(id)) {
            QPointF pos = getNodePosition(w.getLocationNode());
            QGraphicsRectItem* rect = m_scene->addRect(0, 0, 40, 40, Qt::NoPen, QBrush(QColor(255, 140, 0)));
            rect->setPos(pos - QPointF(20, 20));
            rect->setZValue(1);
            
            QGraphicsTextItem* label = m_scene->addText(QString("WH %1").arg(id));
            label->setDefaultTextColor(Qt::white);
            label->setPos(pos - QPointF(20, 40));
            
            m_warehouseItems[id] = nullptr;
        }
    }
    
    // 2. Draw Orders
    qDeleteAll(m_orderItems);
    m_orderItems.clear();
    
    const auto& orders = m_simulator->getOrders();
    for (const auto& pair : orders) {
        const Order& o = pair.second;
        if (o.getStatus() == OrderStatus::Waiting || o.getStatus() == OrderStatus::Assigned) {
            QPointF pos = getNodePosition(o.getDestination());
            
            // Bigger visual for orders
            QColor color = (o.getPriorityClass() == PriorityClass::VIP) ? Qt::yellow : Qt::cyan;
            QGraphicsRectItem* item = m_scene->addRect(0, 0, 14, 14, QPen(Qt::white), QBrush(color));
            item->setPos(pos - QPointF(7, 7));
            item->setZValue(1.5);
            item->setToolTip(QString("Order #%1 (Due: %2)").arg(o.getId()).arg(o.getDueBy()));
            
            m_orderItems[o.getId()] = item;
        }
    }
    
    // 3. Update Vehicles & Paths
    const auto& vehicles = m_simulator->getVehicles();
    for (const auto& pair : vehicles) {
        int vid = pair.first;
        const Vehicle& v = pair.second;
        
        // Ensure vehicle item exists
        if (!m_vehicleItems.contains(vid)) {
            QColor vColor = v.isRefrigerated() ? QColor(0, 122, 204) : QColor(46, 204, 113); // Brighter Blue/Green
            QGraphicsEllipseItem* item = m_scene->addEllipse(0, 0, 24, 24, QPen(Qt::white, 2), QBrush(vColor));
            item->setZValue(3);
            item->setToolTip(QString("Vehicle #%1").arg(vid));
            
            m_vehicleItems[vid] = item;
            
            // Path Line
            QGraphicsLineItem* line = m_scene->addLine(0,0,0,0, QPen(vColor, 2, Qt::DashLine));
            line->setZValue(0.5);
            line->setVisible(false);
            m_pathLines[vid] = line;
            
            // Init state
            int startNodeId = 0;
            if (warehouses.count(v.getHomeWarehouse())) startNodeId = warehouses.at(v.getHomeWarehouse()).getLocationNode();
            m_vehicleStates[vid].currentPos = getNodePosition(startNodeId);
            m_vehicleStates[vid].targetPos = m_vehicleStates[vid].currentPos;
            item->setPos(m_vehicleStates[vid].currentPos - QPointF(12, 12));
        }
        
        // Update Target logic
        int targetNode = -1;
        if (v.getStatus() == VehicleStatus::Outbound) targetNode = v.getCurrentDestination();
        else if (v.getStatus() == VehicleStatus::Returning) {
            if (warehouses.count(v.getHomeWarehouse())) targetNode = warehouses.at(v.getHomeWarehouse()).getLocationNode();
        }
        
        if (targetNode != -1) {
            m_vehicleStates[vid].targetPos = getNodePosition(targetNode);
            m_pathLines[vid]->setLine(QLineF(m_vehicleStates[vid].currentPos, m_vehicleStates[vid].targetPos));
            m_pathLines[vid]->setVisible(true);
        } else {
            m_pathLines[vid]->setVisible(false);
            // If idle, target = current
            VehicleAnimState& state = m_vehicleStates[vid];
            QPointF diff = state.targetPos - state.currentPos;
            if (QPointF::dotProduct(diff, diff) < 1.0) {
                 // Already at target, stay there?
                 // Usually vehicle stays at node unless moving.
                 // We rely on simulator logic mainly.
            }
        }
    }
}

void MapWidget::updateAnimation() {
    for (auto it = m_vehicleItems.begin(); it != m_vehicleItems.end(); ++it) {
        int vid = it.key();
        QGraphicsItem* item = it.value();
        VehicleAnimState& state = m_vehicleStates[vid];
        QGraphicsLineItem* line = m_pathLines.value(vid);
        
        QPointF diff = state.targetPos - state.currentPos;
        double dist = std::sqrt(diff.x()*diff.x() + diff.y()*diff.y());
        
        if (dist > 1.0) {
            double speed = 4.0; 
            QPointF move = diff * (speed / dist);
            state.currentPos += move;
            
            // Update line to start from current pos
            if (line && line->isVisible()) {
                line->setLine(QLineF(state.currentPos, state.targetPos));
            }
        } else {
            state.currentPos = state.targetPos;
            if (line) line->setVisible(false); // Hide line when arrived
        }
        
        item->setPos(state.currentPos - QPointF(12, 12));
    }
}

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
    
    // Initialize node positions (simple layout for now)
    // We assume 100 potential nodes mapped to the grid
    int rows = SCENE_HEIGHT / 100;
    int cols = SCENE_WIDTH / 100;
    
    for (int i = 0; i < 100; ++i) {
        int r = (i / cols) % rows;
        int c = i % cols;
        // Add some jitter so it looks organic
        int x = c * 100 + 50 + QRandomGenerator::global()->bounded(-20, 20);
        int y = r * 100 + 50 + QRandomGenerator::global()->bounded(-20, 20);
        m_nodePositions[i] = QPointF(x, y);
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
        // Fallback for unknown nodes
        m_nodePositions[nodeId] = QPointF(SCENE_WIDTH/2, SCENE_HEIGHT/2);
    }
    return m_nodePositions[nodeId];
}

void MapWidget::refresh() {
    // Clear dynamic items (vehicles, orders) but keep grid/warehouses if static
    // Actually, simple approach: remove known dynamic items
    
    // 1. Draw Warehouses
    const auto& warehouses = m_simulator->getWarehouses();
    for (const auto& pair : warehouses) {
        int id = pair.first;
        const Warehouse& w = pair.second;
        
        if (!m_warehouseItems.contains(id)) {
            QPointF pos = getNodePosition(w.getLocationNode());
            
            // Draw Warehouse Icon (Rect for now)
            QGraphicsRectItem* rect = m_scene->addRect(0, 0, 40, 40, Qt::NoPen, QBrush(QColor(255, 140, 0))); // Dark Orange
            rect->setPos(pos - QPointF(20, 20));
            rect->setZValue(1); // Bottom layer
            
            // Label
            QGraphicsTextItem* label = m_scene->addText(QString("WH %1").arg(id));
            label->setDefaultTextColor(Qt::white);
            label->setPos(pos - QPointF(20, 40));
            
            m_warehouseItems[id] = nullptr; // Just marking as created
        }
    }
    
    // 2. Draw Orders (Pending)
    // Remove old order items
    qDeleteAll(m_orderItems);
    m_orderItems.clear();
    
    const auto& orders = m_simulator->getOrders();
    for (const auto& pair : orders) {
        const Order& o = pair.second;
        if (o.getStatus() == OrderStatus::Waiting || o.getStatus() == OrderStatus::Assigned) {
            QPointF pos = getNodePosition(o.getDestination());
            
            QColor color = (o.getPriorityClass() == PriorityClass::VIP) ? Qt::yellow : Qt::cyan;
            QGraphicsRectItem* item = m_scene->addRect(0, 0, 10, 10, Qt::NoPen, QBrush(color));
            item->setPos(pos - QPointF(5, 5));
            item->setToolTip(QString("Order #%1 (Due: %2)").arg(o.getId()).arg(o.getDueBy()));
            
            m_orderItems[o.getId()] = item;
        }
    }
    
    // 3. Update Vehicles
    const auto& vehicles = m_simulator->getVehicles();
    for (const auto& pair : vehicles) {
        int vid = pair.first;
        const Vehicle& v = pair.second;
        
        // Ensure vehicle item exists
        if (!m_vehicleItems.contains(vid)) {
            QColor vColor = v.isRefrigerated() ? QColor(100, 149, 237) : QColor(144, 238, 144); // Blue/Green
            QGraphicsEllipseItem* item = m_scene->addEllipse(0, 0, 20, 20, QPen(Qt::white), QBrush(vColor));
            item->setZValue(2); // Top layer
            item->setToolTip(QString("Vehicle #%1").arg(vid));
            
            m_vehicleItems[vid] = item;
            
            // Init state
            int startNode = v.getHomeWarehouse(); // Assuming home loc logic
             // Ideally map warehouse ID to node ID. For now assume warehouse ID has a node.
             // Wait, warehouse stores its locationNode.
             // We need to look up warehouse to get node.
            int startNodeId = 0; // Default
            if (warehouses.count(v.getHomeWarehouse())) {
                startNodeId = warehouses.at(v.getHomeWarehouse()).getLocationNode();
            }
            m_vehicleStates[vid].currentPos = getNodePosition(startNodeId);
            m_vehicleStates[vid].targetPos = m_vehicleStates[vid].currentPos;
            item->setPos(m_vehicleStates[vid].currentPos - QPointF(10, 10));
        }
        
        // Update Target
        VehicleAnimState& state = m_vehicleStates[vid];
        QPointF target;
        
        if (v.getStatus() == VehicleStatus::Outbound) {
            target = getNodePosition(v.getCurrentDestination());
        } else if (v.getStatus() == VehicleStatus::Returning) {
            // Find home node
            int homeNode = 0;
            if (warehouses.count(v.getHomeWarehouse())) {
                homeNode = warehouses.at(v.getHomeWarehouse()).getLocationNode();
            }
            target = getNodePosition(homeNode);
        } else {
             // At a warehouse or location
             // If Available, it's at home
             int homeNode = 0;
             if (warehouses.count(v.getHomeWarehouse())) homeNode = warehouses.at(v.getHomeWarehouse()).getLocationNode();
             target = getNodePosition(homeNode);
        }
        
        state.targetPos = target;
    }
}

void MapWidget::updateAnimation() {
    // Smoothly interpolate vehicles towards target
    for (auto it = m_vehicleItems.begin(); it != m_vehicleItems.end(); ++it) {
        int vid = it.key();
        QGraphicsItem* item = it.value();
        VehicleAnimState& state = m_vehicleStates[vid];
        
        QPointF diff = state.targetPos - state.currentPos;
        double dist = std::sqrt(diff.x()*diff.x() + diff.y()*diff.y());
        
        if (dist > 1.0) {
            // Move towards target
            double speed = 5.0; // Pxls per frame
            QPointF move = diff * (speed / dist);
            state.currentPos += move;
        } else {
            state.currentPos = state.targetPos;
        }
        
        item->setPos(state.currentPos - QPointF(10, 10)); // Center offset
    }
}

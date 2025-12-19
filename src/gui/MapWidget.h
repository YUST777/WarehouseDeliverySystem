#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMap>
#include <QTimer>
#include "../core/Simulator.h"

// Forward declarations
class WarehouseItem;
class VehicleItem;
class OrderItem;

class MapWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapWidget(Simulator* simulator, QWidget* parent = nullptr);
    ~MapWidget();

    void refresh(); // Full refresh of the map state

public slots:
    void updateAnimation(); // Called periodically for smooth movement

private:
    void setupScene();
    void drawGrid();
    QPointF getNodePosition(int nodeId);

    Simulator* m_simulator;
    QGraphicsScene* m_scene;
    QTimer* m_animationTimer;

    // Cache
    QMap<int, QPointF> m_nodePositions;
    QMap<int, QGraphicsPixmapItem*> m_warehouseItems;
    QMap<int, QGraphicsEllipseItem*> m_vehicleItems; // Using simple shapes for now, or pixmaps
    QMap<int, QGraphicsRectItem*> m_orderItems;
    
    // Animation state
    struct VehicleAnimState {
        QPointF currentPos;
        QPointF targetPos;
    };
    QMap<int, VehicleAnimState> m_vehicleStates;
};

#endif // MAPWIDGET_H

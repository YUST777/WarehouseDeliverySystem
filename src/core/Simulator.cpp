#include "Simulator.h"
#include "io/InputParser.h"
#include "io/OutputWriter.h"
#include <algorithm>

Simulator::Simulator(QObject* parent)
    : QObject(parent), m_currentTime(0), m_isRunning(false), 
      m_speedMs(500), m_numWarehouses(0), m_numItems(0), m_numVehicles(0) {
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Simulator::onTimerTick);
    
    m_scheduler.setData(&m_orders, &m_warehouses, &m_vehicles, &m_travelTimes);
}

Simulator::~Simulator() {
    delete m_timer;
}

bool Simulator::loadFromFile(const QString& filename) {
    InputParser parser;
    if (!parser.parse(filename.toStdString())) {
        return false;
    }
    
    m_numWarehouses = parser.getNumWarehouses();
    m_numItems = parser.getNumItems();
    m_numVehicles = parser.getNumVehicles();
    m_travelTimes = parser.getTravelTimes();
    m_warehouses = parser.getWarehouses();
    m_vehicles = parser.getVehicles();
    
    // Load events into event manager
    for (const auto& event : parser.getEvents()) {
        m_eventManager.addEvent(event);
    }
    
    m_currentTime = 0;
    m_isRunning = false;
    m_orders.clear();
    m_deliveredOrders.clear();
    
    emit logMessage(QString("Loaded: %1 warehouses, %2 items, %3 vehicles")
        .arg(m_numWarehouses).arg(m_numItems).arg(m_numVehicles));
    
    return true;
}

bool Simulator::saveResults(const QString& filename) {
    OutputWriter writer;
    return writer.write(filename.toStdString(), m_orders, m_deliveredOrders, getStatistics());
}

void Simulator::step() {
    // Process all events at current time
    processEvents();
    
    // Process vehicle arrivals
    auto deliveries = m_scheduler.processVehicleArrivals(m_currentTime);
    for (const auto& delivery : deliveries) {
        m_deliveredOrders.push_back(delivery.orderId);
        emit orderDelivered(delivery.orderId);
        emit logMessage(QString("T=%1: Order #%2 delivered").arg(m_currentTime).arg(delivery.orderId));
    }
    
    // Attempt to assign waiting orders
    auto assignments = m_scheduler.attemptAssignments(m_currentTime);
    for (const auto& assignment : assignments) {
        emit vehicleDispatched(assignment.vehicleId, assignment.orderId);
        emit logMessage(QString("T=%1: Order #%2 dispatched via Vehicle #%3 from Warehouse #%4")
            .arg(m_currentTime).arg(assignment.orderId)
            .arg(assignment.vehicleId).arg(assignment.warehouseId));
    }
    
    // Check if simulation is finished
    if (isFinished()) {
        pause();
        emit simulationFinished();
        emit logMessage("Simulation completed!");
        return;
    }
    
    // Advance time
    m_currentTime++;
    emit timeAdvanced(m_currentTime);
}

void Simulator::run() {
    m_isRunning = true;
    m_timer->start(m_speedMs);
}

void Simulator::pause() {
    m_isRunning = false;
    m_timer->stop();
}

void Simulator::reset() {
    pause();
    m_currentTime = 0;
    m_orders.clear();
    m_deliveredOrders.clear();
    m_eventManager.clear();
    emit timeAdvanced(0);
    emit logMessage("Simulation reset");
}

bool Simulator::isFinished() const {
    return !m_eventManager.hasEvents() && 
           !m_scheduler.hasWaitingOrders() &&
           std::all_of(m_vehicles.begin(), m_vehicles.end(),
               [](const auto& v) { return v.second.getStatus() == VehicleStatus::Available; });
}

void Simulator::onTimerTick() {
    step();
}

void Simulator::processEvents() {
    auto events = m_eventManager.getEventsAt(m_currentTime);
    
    for (const auto& event : events) {
        switch (event->getType()) {
            case EventType::OrderArrival:
                processOrderArrival(dynamic_cast<OrderArrivalEvent*>(event.get()));
                break;
            case EventType::Restock:
                processRestock(dynamic_cast<RestockEvent*>(event.get()));
                break;
            case EventType::Cancel:
                processCancel(dynamic_cast<CancelEvent*>(event.get()));
                break;
            case EventType::Maintenance:
                processMaintenance(dynamic_cast<MaintenanceEvent*>(event.get()));
                break;
            case EventType::Reroute:
                processReroute(dynamic_cast<RerouteEvent*>(event.get()));
                break;
        }
    }
}

void Simulator::processOrderArrival(OrderArrivalEvent* event) {
    if (!event) return;
    
    double value = 0;
    for (const auto& item : event->getDemand()) {
        value += item.second;  // Simple: value = total quantity
    }
    
    Order order(event->getOrderId(), m_currentTime, event->getDueBy(),
                event->getDestination(),
                event->isVip() ? PriorityClass::VIP : PriorityClass::Standard,
                value, event->getDemand());
    
    m_orders[event->getOrderId()] = order;
    
    if (event->isVip()) {
        m_scheduler.addVipOrder(event->getOrderId());
    } else {
        m_scheduler.addStandardOrder(event->getOrderId());
    }
    
    emit orderArrived(event->getOrderId());
    emit logMessage(QString("T=%1: %2").arg(m_currentTime).arg(QString::fromStdString(event->getDescription())));
}

void Simulator::processRestock(RestockEvent* event) {
    if (!event) return;
    
    Warehouse& warehouse = m_warehouses[event->getWarehouseId()];
    for (const auto& item : event->getItems()) {
        warehouse.addInventory(item.first, item.second);
    }
    
    emit inventoryRestocked(event->getWarehouseId());
    emit logMessage(QString("T=%1: %2").arg(m_currentTime).arg(QString::fromStdString(event->getDescription())));
}

void Simulator::processCancel(CancelEvent* event) {
    if (!event) return;
    
    auto it = m_orders.find(event->getOrderId());
    if (it != m_orders.end() && it->second.getStatus() == OrderStatus::Waiting) {
        it->second.setStatus(OrderStatus::Canceled);
        m_scheduler.removeFromQueues(event->getOrderId());
        emit orderCanceled(event->getOrderId());
        emit logMessage(QString("T=%1: %2").arg(m_currentTime).arg(QString::fromStdString(event->getDescription())));
    }
}

void Simulator::processMaintenance(MaintenanceEvent* event) {
    if (!event) return;
    
    auto it = m_vehicles.find(event->getVehicleId());
    if (it != m_vehicles.end() && it->second.getStatus() == VehicleStatus::Available) {
        it->second.setStatus(VehicleStatus::Maintenance);
        it->second.setAvailableTime(m_currentTime + event->getDuration());
        emit logMessage(QString("T=%1: %2").arg(m_currentTime).arg(QString::fromStdString(event->getDescription())));
    }
}

void Simulator::processReroute(RerouteEvent* event) {
    if (!event) return;
    
    int a = event->getNodeA();
    int b = event->getNodeB();
    if (a < static_cast<int>(m_travelTimes.size()) && b < static_cast<int>(m_travelTimes[a].size())) {
        m_travelTimes[a][b] = event->getNewTime();
        m_travelTimes[b][a] = event->getNewTime();
        emit logMessage(QString("T=%1: %2").arg(m_currentTime).arg(QString::fromStdString(event->getDescription())));
    }
}

Simulator::Statistics Simulator::getStatistics() const {
    Statistics stats;
    stats.totalOrders = m_orders.size();
    
    double totalWait = 0, totalTransit = 0;
    int onTime = 0;
    
    for (const auto& [oid, order] : m_orders) {
        if (order.getPriorityClass() == PriorityClass::VIP) stats.vipOrders++;
        else stats.stdOrders++;
        
        if (order.getStatus() == OrderStatus::Delivered) {
            stats.deliveredOrders++;
            stats.totalValue += order.getValue();
            
            int waitTime = order.getAssignTime() - order.getRequestTime();
            int transitTime = order.getFinishTime() - order.getDispatchTime();
            totalWait += waitTime;
            totalTransit += transitTime;
            
            if (order.getDueBy() == 0 || order.getFinishTime() <= order.getDueBy()) {
                onTime++;
            }
        } else if (order.getStatus() == OrderStatus::Canceled) {
            stats.canceledOrders++;
        }
    }
    
    if (stats.deliveredOrders > 0) {
        stats.avgWaitTime = totalWait / stats.deliveredOrders;
        stats.avgTransitTime = totalTransit / stats.deliveredOrders;
        stats.onTimeRate = static_cast<double>(onTime) / stats.deliveredOrders * 100;
    }
    
    return stats;
}

void Simulator::addManualOrder(int orderId, int dest, int dueBy, bool isVip,
                               const std::vector<std::pair<int, int>>& items) {
    // Initialize empty simulation if no data loaded
    if (m_warehouses.empty()) {
        initializeEmpty();
    }
    
    double value = 0;
    for (const auto& item : items) {
        value += item.second;
    }
    
    Order order(orderId, m_currentTime, dueBy, dest,
                isVip ? PriorityClass::VIP : PriorityClass::Standard,
                value, items);
    
    m_orders[orderId] = order;
    
    if (isVip) {
        m_scheduler.addVipOrder(orderId);
    } else {
        m_scheduler.addStandardOrder(orderId);
    }
    
    emit orderArrived(orderId);
    emit logMessage(QString("T=%1: Manual order #%2 added (%3)")
        .arg(m_currentTime).arg(orderId).arg(isVip ? "VIP" : "Standard"));
}

void Simulator::cancelOrder(int orderId) {
    auto it = m_orders.find(orderId);
    if (it != m_orders.end() && it->second.getStatus() == OrderStatus::Waiting) {
        it->second.setStatus(OrderStatus::Canceled);
        m_scheduler.removeFromQueues(orderId);
        emit orderCanceled(orderId);
        emit logMessage(QString("T=%1: Order #%2 canceled").arg(m_currentTime).arg(orderId));
    }
}

void Simulator::initializeEmpty(int numWarehouses, int numItems, int numVehicles) {
    m_numWarehouses = numWarehouses;
    m_numItems = numItems;
    m_numVehicles = numVehicles;
    
    // Create travel time matrix
    m_travelTimes.resize(numWarehouses + 1, std::vector<int>(numWarehouses + 1, 10));
    for (int i = 0; i <= numWarehouses; ++i) {
        m_travelTimes[i][i] = 0;
    }
    
    // Create warehouses with initial inventory
    for (int i = 1; i <= numWarehouses; ++i) {
        m_warehouses[i] = Warehouse(i, i);
        for (int j = 1; j <= numItems; ++j) {
            m_warehouses[i].setInventory(j, 100);  // 100 units each
        }
    }
    
    // Create vehicles
    int vid = 1;
    for (int w = 1; w <= numWarehouses; ++w) {
        // One standard vehicle per warehouse
        m_vehicles[vid] = Vehicle(vid, VehicleType::Standard, 2, 100, w);
        vid++;
    }
    // Add one more vehicle
    if (vid <= numVehicles) {
        m_vehicles[vid] = Vehicle(vid, VehicleType::Refrigerated, 3, 80, 1);
    }
    
    emit logMessage(QString("Initialized empty simulation: %1 warehouses, %2 items, %3 vehicles")
        .arg(numWarehouses).arg(numItems).arg(m_vehicles.size()));
}


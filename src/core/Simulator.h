#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QTimer>
#include <map>
#include <vector>
#include "EventManager.h"
#include "Scheduler.h"
#include "models/Order.h"
#include "models/Warehouse.h"
#include "models/Vehicle.h"

class Simulator : public QObject {
    Q_OBJECT
    
public:
    explicit Simulator(QObject* parent = nullptr);
    ~Simulator();
    
    // Data loading
    bool loadFromFile(const QString& filename);
    bool saveResults(const QString& filename);
    
    // Simulation control
    void step();           // Advance one timestep
    void run();            // Start continuous simulation
    void pause();          // Pause simulation
    void reset();          // Reset to initial state
    
    // Speed control (ms between steps when running)
    void setSpeed(int msPerStep) { m_speedMs = msPerStep; }
    int getSpeed() const { return m_speedMs; }
    
    // State queries
    int getCurrentTime() const { return m_currentTime; }
    bool isRunning() const { return m_isRunning; }
    bool isFinished() const;
    
    // Data access for GUI
    const std::map<int, Order>& getOrders() const { return m_orders; }
    const std::map<int, Warehouse>& getWarehouses() const { return m_warehouses; }
    const std::map<int, Vehicle>& getVehicles() const { return m_vehicles; }
    const std::vector<int>& getDeliveredOrders() const { return m_deliveredOrders; }
    
    // Queue access
    std::vector<int> getVipQueue() const { return m_scheduler.getVipQueue(); }
    std::vector<int> getStdQueue() const { return m_scheduler.getStandardQueue(); }
    
    // Manual order management (GUI input)
    void addManualOrder(int orderId, int dest, int dueBy, bool isVip,
                        const std::vector<std::pair<int, int>>& items);
    void cancelOrder(int orderId);
    
    // Initialize empty simulation for manual input
    void initializeEmpty(int numWarehouses = 3, int numItems = 10, int numVehicles = 4);
    
    // Statistics
    struct Statistics {
        int totalOrders = 0;
        int deliveredOrders = 0;
        int canceledOrders = 0;
        int vipOrders = 0;
        int stdOrders = 0;
        double totalValue = 0;
        double avgWaitTime = 0;
        double avgTransitTime = 0;
        double onTimeRate = 0;
    };
    Statistics getStatistics() const;
    
signals:
    void timeAdvanced(int newTime);
    void orderArrived(int orderId);
    void orderDelivered(int orderId);
    void orderCanceled(int orderId);
    void vehicleDispatched(int vehicleId, int orderId);
    void vehicleReturned(int vehicleId);
    void inventoryRestocked(int warehouseId);
    void simulationFinished();
    void logMessage(const QString& message);
    
private slots:
    void onTimerTick();
    
private:
    void processEvents();
    void processOrderArrival(OrderArrivalEvent* event);
    void processRestock(RestockEvent* event);
    void processCancel(CancelEvent* event);
    void processMaintenance(MaintenanceEvent* event);
    void processReroute(RerouteEvent* event);
    
    // Time management
    int m_currentTime;
    bool m_isRunning;
    int m_speedMs;
    QTimer* m_timer;
    
    // Core components
    EventManager m_eventManager;
    Scheduler m_scheduler;
    
    // Data storage
    std::map<int, Order> m_orders;
    std::map<int, Warehouse> m_warehouses;
    std::map<int, Vehicle> m_vehicles;
    std::vector<std::vector<int>> m_travelTimes;
    
    // Tracking
    std::vector<int> m_deliveredOrders;
    int m_numWarehouses;
    int m_numItems;
    int m_numVehicles;
};

#endif // SIMULATOR_H

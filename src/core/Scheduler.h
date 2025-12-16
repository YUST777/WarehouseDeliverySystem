#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <map>
#include <functional>
#include "models/Order.h"
#include "models/Warehouse.h"
#include "models/Vehicle.h"

class Scheduler {
public:
    Scheduler();
    
    // Set data references
    void setData(std::map<int, Order>* orders,
                 std::map<int, Warehouse>* warehouses,
                 std::map<int, Vehicle>* vehicles,
                 std::vector<std::vector<int>>* travelTimes);
    
    // Order queue management
    void addVipOrder(int orderId);
    void addStandardOrder(int orderId);
    void removeFromQueues(int orderId);
    
    // Main scheduling function - attempts to assign orders to vehicles
    struct AssignmentResult {
        int orderId;
        int warehouseId;
        int vehicleId;
        int estimatedDeliveryTime;
    };
    
    std::vector<AssignmentResult> attemptAssignments(int currentTime);
    
    // Process vehicle arrivals and returns
    struct DeliveryResult {
        int orderId;
        int finishTime;
    };
    std::vector<DeliveryResult> processVehicleArrivals(int currentTime);
    
    // Queue inspection
    std::vector<int> getVipQueue() const;
    std::vector<int> getStandardQueue() const;
    int getVipQueueSize() const { return m_vipQueue.size(); }
    int getStandardQueueSize() const { return m_stdQueue.size(); }
    
    // Check if any orders are waiting
    bool hasWaitingOrders() const { return !m_vipQueue.empty() || !m_stdQueue.empty(); }
    
private:
    // Find best warehouse to fulfill an order
    int findBestWarehouse(const Order& order) const;
    
    // Find best available vehicle at a warehouse for an order
    int findBestVehicle(int warehouseId, const Order& order) const;
    
    // Calculate travel time
    int getTravelTime(int from, int to) const;
    
    // Data references (owned by Simulator)
    std::map<int, Order>* m_orders;
    std::map<int, Warehouse>* m_warehouses;
    std::map<int, Vehicle>* m_vehicles;
    std::vector<std::vector<int>>* m_travelTimes;
    
    // Order queues
    std::vector<int> m_vipQueue;
    std::vector<int> m_stdQueue;
};

#endif // SCHEDULER_H

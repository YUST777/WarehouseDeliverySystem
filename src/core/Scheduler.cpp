#include "Scheduler.h"
#include <algorithm>
#include <limits>

Scheduler::Scheduler() 
    : m_orders(nullptr), m_warehouses(nullptr), 
      m_vehicles(nullptr), m_travelTimes(nullptr) {}

void Scheduler::setData(std::map<int, Order>* orders,
                        std::map<int, Warehouse>* warehouses,
                        std::map<int, Vehicle>* vehicles,
                        std::vector<std::vector<int>>* travelTimes) {
    m_orders = orders;
    m_warehouses = warehouses;
    m_vehicles = vehicles;
    m_travelTimes = travelTimes;
}

void Scheduler::addVipOrder(int orderId) {
    m_vipQueue.push_back(orderId);
}

void Scheduler::addStandardOrder(int orderId) {
    m_stdQueue.push_back(orderId);
}

void Scheduler::removeFromQueues(int orderId) {
    m_vipQueue.erase(std::remove(m_vipQueue.begin(), m_vipQueue.end(), orderId), m_vipQueue.end());
    m_stdQueue.erase(std::remove(m_stdQueue.begin(), m_stdQueue.end(), orderId), m_stdQueue.end());
}

std::vector<Scheduler::AssignmentResult> Scheduler::attemptAssignments(int currentTime) {
    std::vector<AssignmentResult> results;
    
    // Sort VIP queue by priority
    std::sort(m_vipQueue.begin(), m_vipQueue.end(), 
        [this, currentTime](int a, int b) {
            return m_orders->at(a).calculatePriority(currentTime) > 
                   m_orders->at(b).calculatePriority(currentTime);
        });
    
    auto tryAssign = [&](int orderId) -> bool {
        Order& order = m_orders->at(orderId);
        if (order.getStatus() != OrderStatus::Waiting) return false;
        
        int warehouseId = findBestWarehouse(order);
        if (warehouseId == -1) return false;
        
        int vehicleId = findBestVehicle(warehouseId, order);
        if (vehicleId == -1) return false;
        
        // Perform assignment
        Warehouse& warehouse = m_warehouses->at(warehouseId);
        Vehicle& vehicle = m_vehicles->at(vehicleId);
        
        // Deduct inventory
        for (const auto& item : order.getDemand()) {
            warehouse.removeInventory(item.first, item.second);
        }
        
        // Update order
        order.setStatus(OrderStatus::InTransit);
        order.setAssignedWarehouse(warehouseId);
        order.setAssignedVehicle(vehicleId);
        order.setAssignTime(currentTime);
        order.setDispatchTime(currentTime);
        
        // Update vehicle
        vehicle.setStatus(VehicleStatus::Outbound);
        vehicle.setCurrentDestination(order.getDestination());
        vehicle.assignOrder(orderId);
        
        int travelTime = getTravelTime(warehouseId, order.getDestination());
        int arrivalTime = currentTime + std::max(1, travelTime / vehicle.getSpeed());
        vehicle.setAvailableTime(arrivalTime);
        
        AssignmentResult result;
        result.orderId = orderId;
        result.warehouseId = warehouseId;
        result.vehicleId = vehicleId;
        result.estimatedDeliveryTime = arrivalTime;
        results.push_back(result);
        
        return true;
    };
    
    // Try VIP orders first
    for (auto it = m_vipQueue.begin(); it != m_vipQueue.end();) {
        if (m_orders->at(*it).getStatus() != OrderStatus::Waiting) {
            it = m_vipQueue.erase(it);
        } else if (tryAssign(*it)) {
            it = m_vipQueue.erase(it);
        } else {
            ++it;
        }
    }
    
    // Then standard orders
    for (auto it = m_stdQueue.begin(); it != m_stdQueue.end();) {
        if (m_orders->at(*it).getStatus() != OrderStatus::Waiting) {
            it = m_stdQueue.erase(it);
        } else if (tryAssign(*it)) {
            it = m_stdQueue.erase(it);
        } else {
            ++it;
        }
    }
    
    return results;
}

std::vector<Scheduler::DeliveryResult> Scheduler::processVehicleArrivals(int currentTime) {
    std::vector<DeliveryResult> results;
    
    for (auto& [vid, vehicle] : *m_vehicles) {
        if (vehicle.getAvailableTime() <= currentTime) {
            if (vehicle.getStatus() == VehicleStatus::Outbound) {
                // Vehicle arrived at destination - deliver orders
                for (int orderId : vehicle.getAssignedOrders()) {
                    Order& order = m_orders->at(orderId);
                    order.setStatus(OrderStatus::Delivered);
                    order.setFinishTime(currentTime);
                    
                    DeliveryResult result;
                    result.orderId = orderId;
                    result.finishTime = currentTime;
                    results.push_back(result);
                }
                
                vehicle.clearOrders();
                vehicle.setStatus(VehicleStatus::Returning);
                
                // Calculate return time
                int returnTime = getTravelTime(vehicle.getCurrentDestination(), 
                                              vehicle.getHomeWarehouse());
                vehicle.setAvailableTime(currentTime + std::max(1, returnTime / vehicle.getSpeed()));
                
            } else if (vehicle.getStatus() == VehicleStatus::Returning ||
                       vehicle.getStatus() == VehicleStatus::Maintenance) {
                // Vehicle is back home or maintenance complete
                vehicle.setStatus(VehicleStatus::Available);
            }
        }
    }
    
    return results;
}

int Scheduler::findBestWarehouse(const Order& order) const {
    int bestWarehouse = -1;
    int minDistance = std::numeric_limits<int>::max();
    
    for (const auto& [wid, warehouse] : *m_warehouses) {
        if (warehouse.canFulfillOrder(order.getDemand())) {
            int distance = getTravelTime(wid, order.getDestination());
            if (distance < minDistance) {
                minDistance = distance;
                bestWarehouse = wid;
            }
        }
    }
    
    return bestWarehouse;
}

int Scheduler::findBestVehicle(int warehouseId, const Order& order) const {
    int bestVehicle = -1;
    int earliestAvailable = std::numeric_limits<int>::max();
    
    for (const auto& [vid, vehicle] : *m_vehicles) {
        if (vehicle.getHomeWarehouse() == warehouseId &&
            vehicle.getStatus() == VehicleStatus::Available &&
            vehicle.canCarry(order.getTotalQuantity())) {
            
            if (vehicle.getAvailableTime() < earliestAvailable) {
                earliestAvailable = vehicle.getAvailableTime();
                bestVehicle = vid;
            }
        }
    }
    
    return bestVehicle;
}

int Scheduler::getTravelTime(int from, int to) const {
    if (!m_travelTimes || from < 1 || to < 1 ||
        from >= static_cast<int>(m_travelTimes->size()) ||
        to >= static_cast<int>((*m_travelTimes)[0].size())) {
        return 1;
    }
    return (*m_travelTimes)[from][to];
}

std::vector<int> Scheduler::getVipQueue() const {
    return m_vipQueue;
}

std::vector<int> Scheduler::getStandardQueue() const {
    return m_stdQueue;
}

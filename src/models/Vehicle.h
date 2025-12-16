#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <string>

enum class VehicleType { Standard, Refrigerated };
enum class VehicleStatus { Available, Outbound, Returning, Maintenance };

class Vehicle {
public:
    Vehicle();
    Vehicle(int id, VehicleType type, int speed, int capacity, int homeWarehouse);
    
    // Getters
    int getId() const { return m_id; }
    VehicleType getType() const { return m_type; }
    int getSpeed() const { return m_speed; }
    int getCapacity() const { return m_capacity; }
    int getHomeWarehouse() const { return m_homeWarehouse; }
    VehicleStatus getStatus() const { return m_status; }
    int getAvailableTime() const { return m_availableTime; }
    int getCurrentDestination() const { return m_currentDestination; }
    const std::vector<int>& getAssignedOrders() const { return m_assignedOrders; }
    
    // Setters
    void setStatus(VehicleStatus status) { m_status = status; }
    void setAvailableTime(int time) { m_availableTime = time; }
    void setCurrentDestination(int dest) { m_currentDestination = dest; }
    
    // Order assignment
    void assignOrder(int orderId);
    void clearOrders();
    int getOrderCount() const { return m_assignedOrders.size(); }
    
    // Capacity check
    bool canCarry(int quantity) const;
    int getRemainingCapacity() const;
    
    // Utility
    std::string getTypeString() const;
    std::string getStatusString() const;
    bool isAvailable() const { return m_status == VehicleStatus::Available; }
    bool isRefrigerated() const { return m_type == VehicleType::Refrigerated; }
    
private:
    int m_id;
    VehicleType m_type;
    int m_speed;
    int m_capacity;
    int m_homeWarehouse;
    VehicleStatus m_status;
    int m_availableTime;
    int m_currentDestination;
    std::vector<int> m_assignedOrders;
    int m_usedCapacity;
};

#endif // VEHICLE_H

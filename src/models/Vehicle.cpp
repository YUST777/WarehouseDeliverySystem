#include "Vehicle.h"

Vehicle::Vehicle() 
    : m_id(0), m_type(VehicleType::Standard), m_speed(1), m_capacity(100),
      m_homeWarehouse(0), m_status(VehicleStatus::Available),
      m_availableTime(0), m_currentDestination(-1), m_usedCapacity(0) {}

Vehicle::Vehicle(int id, VehicleType type, int speed, int capacity, int homeWarehouse)
    : m_id(id), m_type(type), m_speed(speed), m_capacity(capacity),
      m_homeWarehouse(homeWarehouse), m_status(VehicleStatus::Available),
      m_availableTime(0), m_currentDestination(-1), m_usedCapacity(0) {}

void Vehicle::assignOrder(int orderId) {
    m_assignedOrders.push_back(orderId);
}

void Vehicle::clearOrders() {
    m_assignedOrders.clear();
    m_usedCapacity = 0;
}

bool Vehicle::canCarry(int quantity) const {
    return (m_usedCapacity + quantity) <= m_capacity;
}

int Vehicle::getRemainingCapacity() const {
    return m_capacity - m_usedCapacity;
}

std::string Vehicle::getTypeString() const {
    return m_type == VehicleType::Refrigerated ? "Refrigerated" : "Standard";
}

std::string Vehicle::getStatusString() const {
    switch (m_status) {
        case VehicleStatus::Available: return "Available";
        case VehicleStatus::Outbound: return "Outbound";
        case VehicleStatus::Returning: return "Returning";
        case VehicleStatus::Maintenance: return "Maintenance";
        default: return "Unknown";
    }
}

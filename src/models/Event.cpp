#include "Event.h"
#include <sstream>

Event::Event(EventType type, int timestamp)
    : m_type(type), m_timestamp(timestamp) {}

// OrderArrivalEvent
OrderArrivalEvent::OrderArrivalEvent(int timestamp, int orderId, int destination,
                                     int dueBy, bool isVip,
                                     const std::vector<std::pair<int, int>>& demand)
    : Event(EventType::OrderArrival, timestamp), m_orderId(orderId),
      m_destination(destination), m_dueBy(dueBy), m_isVip(isVip), m_demand(demand) {}

std::string OrderArrivalEvent::getDescription() const {
    std::stringstream ss;
    ss << "Order #" << m_orderId << " arrived";
    if (m_isVip) ss << " [VIP]";
    ss << " to dest " << m_destination;
    return ss.str();
}

// RestockEvent
RestockEvent::RestockEvent(int timestamp, int warehouseId,
                           const std::vector<std::pair<int, int>>& items)
    : Event(EventType::Restock, timestamp), m_warehouseId(warehouseId), m_items(items) {}

std::string RestockEvent::getDescription() const {
    std::stringstream ss;
    ss << "Restock at Warehouse #" << m_warehouseId << " (" << m_items.size() << " items)";
    return ss.str();
}

// CancelEvent
CancelEvent::CancelEvent(int timestamp, int orderId)
    : Event(EventType::Cancel, timestamp), m_orderId(orderId) {}

std::string CancelEvent::getDescription() const {
    std::stringstream ss;
    ss << "Order #" << m_orderId << " canceled";
    return ss.str();
}

// MaintenanceEvent
MaintenanceEvent::MaintenanceEvent(int timestamp, int vehicleId, int duration)
    : Event(EventType::Maintenance, timestamp), m_vehicleId(vehicleId), m_duration(duration) {}

std::string MaintenanceEvent::getDescription() const {
    std::stringstream ss;
    ss << "Vehicle #" << m_vehicleId << " maintenance for " << m_duration << " timesteps";
    return ss.str();
}

// RerouteEvent
RerouteEvent::RerouteEvent(int timestamp, int nodeA, int nodeB, int newTime)
    : Event(EventType::Reroute, timestamp), m_nodeA(nodeA), m_nodeB(nodeB), m_newTime(newTime) {}

std::string RerouteEvent::getDescription() const {
    std::stringstream ss;
    ss << "Route " << m_nodeA << "-" << m_nodeB << " updated to " << m_newTime << " timesteps";
    return ss.str();
}

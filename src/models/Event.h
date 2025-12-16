#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include <memory>

enum class EventType { 
    OrderArrival,    // R - New order
    Restock,         // S - Inventory restock
    Cancel,          // C - Order cancellation
    Maintenance,     // M - Vehicle maintenance
    Reroute          // U - Route update
};

class Event {
public:
    Event(EventType type, int timestamp);
    virtual ~Event() = default;
    
    EventType getType() const { return m_type; }
    int getTimestamp() const { return m_timestamp; }
    
    virtual std::string getDescription() const = 0;
    
    // Comparison for priority queue (earlier events first)
    bool operator>(const Event& other) const {
        return m_timestamp > other.m_timestamp;
    }
    
protected:
    EventType m_type;
    int m_timestamp;
};

// Order Arrival Event (R)
class OrderArrivalEvent : public Event {
public:
    OrderArrivalEvent(int timestamp, int orderId, int destination, int dueBy,
                      bool isVip, const std::vector<std::pair<int, int>>& demand);
    
    int getOrderId() const { return m_orderId; }
    int getDestination() const { return m_destination; }
    int getDueBy() const { return m_dueBy; }
    bool isVip() const { return m_isVip; }
    const std::vector<std::pair<int, int>>& getDemand() const { return m_demand; }
    
    std::string getDescription() const override;
    
private:
    int m_orderId;
    int m_destination;
    int m_dueBy;
    bool m_isVip;
    std::vector<std::pair<int, int>> m_demand;
};

// Restock Event (S)
class RestockEvent : public Event {
public:
    RestockEvent(int timestamp, int warehouseId, 
                 const std::vector<std::pair<int, int>>& items);
    
    int getWarehouseId() const { return m_warehouseId; }
    const std::vector<std::pair<int, int>>& getItems() const { return m_items; }
    
    std::string getDescription() const override;
    
private:
    int m_warehouseId;
    std::vector<std::pair<int, int>> m_items;
};

// Cancel Event (C)
class CancelEvent : public Event {
public:
    CancelEvent(int timestamp, int orderId);
    
    int getOrderId() const { return m_orderId; }
    std::string getDescription() const override;
    
private:
    int m_orderId;
};

// Maintenance Event (M)
class MaintenanceEvent : public Event {
public:
    MaintenanceEvent(int timestamp, int vehicleId, int duration);
    
    int getVehicleId() const { return m_vehicleId; }
    int getDuration() const { return m_duration; }
    std::string getDescription() const override;
    
private:
    int m_vehicleId;
    int m_duration;
};

// Reroute Event (U)
class RerouteEvent : public Event {
public:
    RerouteEvent(int timestamp, int nodeA, int nodeB, int newTime);
    
    int getNodeA() const { return m_nodeA; }
    int getNodeB() const { return m_nodeB; }
    int getNewTime() const { return m_newTime; }
    std::string getDescription() const override;
    
private:
    int m_nodeA;
    int m_nodeB;
    int m_newTime;
};

// Smart pointer type for polymorphic events
using EventPtr = std::shared_ptr<Event>;

// Comparator for priority queue
struct EventComparator {
    bool operator()(const EventPtr& a, const EventPtr& b) const {
        return a->getTimestamp() > b->getTimestamp();
    }
};

#endif // EVENT_H

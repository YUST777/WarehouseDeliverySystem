#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>

enum class PriorityClass { VIP, Standard };
enum class OrderStatus { Waiting, Assigned, InTransit, Delivered, Canceled, PartiallyFulfilled };

class Order {
public:
    Order();
    Order(int id, int requestTime, int dueBy, int destination, 
          PriorityClass priority, double value, 
          const std::vector<std::pair<int, int>>& demand);
    
    // Getters
    int getId() const { return m_id; }
    int getRequestTime() const { return m_requestTime; }
    int getDueBy() const { return m_dueBy; }
    int getDestination() const { return m_destination; }
    PriorityClass getPriorityClass() const { return m_priority; }
    OrderStatus getStatus() const { return m_status; }
    double getValue() const { return m_value; }
    const std::vector<std::pair<int, int>>& getDemand() const { return m_demand; }
    int getAssignedWarehouse() const { return m_assignedWarehouse; }
    int getAssignedVehicle() const { return m_assignedVehicle; }
    int getAssignTime() const { return m_assignTime; }
    int getDispatchTime() const { return m_dispatchTime; }
    int getFinishTime() const { return m_finishTime; }
    
    // Setters
    void setStatus(OrderStatus status) { m_status = status; }
    void setAssignedWarehouse(int wid) { m_assignedWarehouse = wid; }
    void setAssignedVehicle(int vid) { m_assignedVehicle = vid; }
    void setAssignTime(int time) { m_assignTime = time; }
    void setDispatchTime(int time) { m_dispatchTime = time; }
    void setFinishTime(int time) { m_finishTime = time; }
    
    // Priority calculation
    double calculatePriority(int currentTime) const;
    
    // Utility
    int getTotalQuantity() const;
    std::string getStatusString() const;
    std::string getPriorityString() const;
    
private:
    int m_id;
    int m_requestTime;
    int m_dueBy;
    int m_destination;
    PriorityClass m_priority;
    OrderStatus m_status;
    double m_value;
    std::vector<std::pair<int, int>> m_demand; // ItemID -> Quantity
    
    int m_assignedWarehouse;
    int m_assignedVehicle;
    int m_assignTime;
    int m_dispatchTime;
    int m_finishTime;
};

#endif // ORDER_H

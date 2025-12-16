#include "Order.h"
#include <algorithm>

Order::Order() 
    : m_id(0), m_requestTime(0), m_dueBy(0), m_destination(0),
      m_priority(PriorityClass::Standard), m_status(OrderStatus::Waiting),
      m_value(0), m_assignedWarehouse(-1), m_assignedVehicle(-1),
      m_assignTime(-1), m_dispatchTime(-1), m_finishTime(-1) {}

Order::Order(int id, int requestTime, int dueBy, int destination,
             PriorityClass priority, double value,
             const std::vector<std::pair<int, int>>& demand)
    : m_id(id), m_requestTime(requestTime), m_dueBy(dueBy), 
      m_destination(destination), m_priority(priority),
      m_status(OrderStatus::Waiting), m_value(value), m_demand(demand),
      m_assignedWarehouse(-1), m_assignedVehicle(-1),
      m_assignTime(-1), m_dispatchTime(-1), m_finishTime(-1) {}

double Order::calculatePriority(int currentTime) const {
    // Priority = α * OrderValue / (β * (CurrentTime − RT + 1)) + γ * DeadlineUrgency − δ * SizePenalty
    const double alpha = 1.0;
    const double beta = 1.0;
    const double gamma = 10.0;
    const double delta = 0.1;
    
    double waitFactor = m_value / (beta * (currentTime - m_requestTime + 1.0));
    
    double deadlineUrgency = 0.0;
    if (m_dueBy > 0) {
        int timeLeft = m_dueBy - currentTime;
        if (timeLeft > 0) {
            deadlineUrgency = 1.0 / (timeLeft + 1.0);
        } else {
            deadlineUrgency = 10.0; // Very urgent if past deadline
        }
    }
    
    double sizePenalty = delta * getTotalQuantity();
    
    return alpha * waitFactor + gamma * deadlineUrgency - sizePenalty;
}

int Order::getTotalQuantity() const {
    int total = 0;
    for (const auto& item : m_demand) {
        total += item.second;
    }
    return total;
}

std::string Order::getStatusString() const {
    switch (m_status) {
        case OrderStatus::Waiting: return "Waiting";
        case OrderStatus::Assigned: return "Assigned";
        case OrderStatus::InTransit: return "In Transit";
        case OrderStatus::Delivered: return "Delivered";
        case OrderStatus::Canceled: return "Canceled";
        case OrderStatus::PartiallyFulfilled: return "Partial";
        default: return "Unknown";
    }
}

std::string Order::getPriorityString() const {
    return m_priority == PriorityClass::VIP ? "VIP" : "Standard";
}

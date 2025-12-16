#include "Warehouse.h"
#include <algorithm>

Warehouse::Warehouse() : m_id(0), m_locationNode(0) {}

Warehouse::Warehouse(int id, int locationNode) 
    : m_id(id), m_locationNode(locationNode) {}

int Warehouse::getInventory(int itemId) const {
    auto it = m_inventory.find(itemId);
    return (it != m_inventory.end()) ? it->second : 0;
}

void Warehouse::setInventory(int itemId, int quantity) {
    m_inventory[itemId] = quantity;
}

void Warehouse::addInventory(int itemId, int quantity) {
    m_inventory[itemId] += quantity;
}

bool Warehouse::removeInventory(int itemId, int quantity) {
    auto it = m_inventory.find(itemId);
    if (it == m_inventory.end() || it->second < quantity) {
        return false;
    }
    it->second -= quantity;
    return true;
}

bool Warehouse::hasStock(int itemId, int quantity) const {
    return getInventory(itemId) >= quantity;
}

bool Warehouse::canFulfillOrder(const std::vector<std::pair<int, int>>& demand) const {
    for (const auto& item : demand) {
        if (!hasStock(item.first, item.second)) {
            return false;
        }
    }
    return true;
}

void Warehouse::addToDispatchQueue(int orderId) {
    m_dispatchQueue.push(orderId);
}

int Warehouse::getNextDispatch() {
    if (m_dispatchQueue.empty()) return -1;
    int orderId = m_dispatchQueue.front();
    m_dispatchQueue.pop();
    return orderId;
}

int Warehouse::getTotalItems() const {
    int total = 0;
    for (const auto& item : m_inventory) {
        total += item.second;
    }
    return total;
}

std::vector<std::pair<int, int>> Warehouse::getLowStockItems(int threshold) const {
    std::vector<std::pair<int, int>> lowStock;
    for (const auto& item : m_inventory) {
        if (item.second < threshold) {
            lowStock.push_back(item);
        }
    }
    return lowStock;
}

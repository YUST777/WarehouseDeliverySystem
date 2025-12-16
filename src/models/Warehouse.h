#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <unordered_map>
#include <queue>
#include <vector>

class Warehouse {
public:
    Warehouse();
    Warehouse(int id, int locationNode);
    
    // Getters
    int getId() const { return m_id; }
    int getLocationNode() const { return m_locationNode; }
    int getInventory(int itemId) const;
    const std::unordered_map<int, int>& getAllInventory() const { return m_inventory; }
    const std::queue<int>& getDispatchQueue() const { return m_dispatchQueue; }
    
    // Inventory operations
    void setInventory(int itemId, int quantity);
    void addInventory(int itemId, int quantity);
    bool removeInventory(int itemId, int quantity);
    bool hasStock(int itemId, int quantity) const;
    bool canFulfillOrder(const std::vector<std::pair<int, int>>& demand) const;
    
    // Dispatch queue operations
    void addToDispatchQueue(int orderId);
    int getNextDispatch();
    bool hasDispatchPending() const { return !m_dispatchQueue.empty(); }
    
    // Statistics
    int getTotalItems() const;
    std::vector<std::pair<int, int>> getLowStockItems(int threshold = 10) const;
    
private:
    int m_id;
    int m_locationNode;
    std::unordered_map<int, int> m_inventory; // ItemID -> Quantity
    std::queue<int> m_dispatchQueue; // Order IDs pending dispatch
};

#endif // WAREHOUSE_H

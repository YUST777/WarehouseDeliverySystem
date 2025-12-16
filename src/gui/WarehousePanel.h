#ifndef WAREHOUSEPANEL_H
#define WAREHOUSEPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <map>
#include "models/Warehouse.h"

class WarehousePanel : public QWidget {
    Q_OBJECT
    
public:
    explicit WarehousePanel(QWidget* parent = nullptr);
    
    void update(const std::map<int, Warehouse>& warehouses);
    
private slots:
    void onWarehouseSelected(int index);
    
private:
    void updateInventoryTable(const Warehouse& warehouse);
    
    const std::map<int, Warehouse>* m_warehousesPtr;
    
    QLabel* m_titleLabel;
    QComboBox* m_warehouseSelector;
    QTableWidget* m_inventoryTable;
    QLabel* m_summaryLabel;
};

#endif // WAREHOUSEPANEL_H

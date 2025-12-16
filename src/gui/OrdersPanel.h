#ifndef ORDERSPANEL_H
#define ORDERSPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <map>
#include <vector>
#include "models/Order.h"

class OrdersPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit OrdersPanel(QWidget* parent = nullptr);
    
    void update(const std::map<int, Order>& orders,
                const std::vector<int>& vipQueue,
                const std::vector<int>& stdQueue);
    
signals:
    void addOrderRequested(int orderId, int dest, int dueBy, bool isVip,
                           const std::vector<std::pair<int, int>>& items);
    void removeOrderRequested(int orderId);
    
private slots:
    void onAddOrderClicked();
    void onRemoveOrderClicked();
    void onAddItemClicked();
    void onRemoveItemClicked();
    
private:
    void setupTable(QTableWidget* table);
    void setupInputForm();
    void populateTable(QTableWidget* table, const std::vector<int>& orderIds,
                       const std::map<int, Order>& orders);
    QColor getStatusColor(OrderStatus status);
    void updateItemsList();
    
    // Tables
    QLabel* m_vipLabel;
    QLabel* m_stdLabel;
    QTableWidget* m_vipTable;
    QTableWidget* m_stdTable;
    
    // Input form
    QGroupBox* m_inputGroup;
    QSpinBox* m_orderIdSpin;
    QSpinBox* m_destSpin;
    QSpinBox* m_dueBySpin;
    QCheckBox* m_vipCheck;
    QPushButton* m_addOrderBtn;
    QPushButton* m_removeOrderBtn;
    
    // Items input
    QSpinBox* m_itemIdSpin;
    QSpinBox* m_itemQtySpin;
    QPushButton* m_addItemBtn;
    QPushButton* m_removeItemBtn;
    QTableWidget* m_itemsTable;
    std::vector<std::pair<int, int>> m_currentItems;
    
    int m_nextOrderId;
};

#endif // ORDERSPANEL_H

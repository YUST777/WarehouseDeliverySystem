#include "WarehousePanel.h"
#include <QHeaderView>
#include <QHBoxLayout>

WarehousePanel::WarehousePanel(QWidget* parent)
    : QWidget(parent), m_warehousesPtr(nullptr) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Header with selector
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    m_titleLabel = new QLabel("🏭 Warehouse Inventory");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    headerLayout->addWidget(m_titleLabel);
    
    headerLayout->addStretch();
    
    QLabel* selectLabel = new QLabel("Select Warehouse:");
    headerLayout->addWidget(selectLabel);
    
    m_warehouseSelector = new QComboBox();
    m_warehouseSelector->setMinimumWidth(150);
    connect(m_warehouseSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WarehousePanel::onWarehouseSelected);
    headerLayout->addWidget(m_warehouseSelector);
    
    mainLayout->addLayout(headerLayout);
    
    // Inventory table
    m_inventoryTable = new QTableWidget();
    m_inventoryTable->setColumnCount(3);
    m_inventoryTable->setHorizontalHeaderLabels({"Item ID", "Quantity", "Status"});
    m_inventoryTable->horizontalHeader()->setStretchLastSection(true);
    m_inventoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_inventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_inventoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_inventoryTable->setAlternatingRowColors(true);
    m_inventoryTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_inventoryTable);
    
    // Summary
    m_summaryLabel = new QLabel("Total Items: 0");
    mainLayout->addWidget(m_summaryLabel);
}

void WarehousePanel::update(const std::map<int, Warehouse>& warehouses) {
    m_warehousesPtr = &warehouses;
    
    // Update selector if needed
    if (m_warehouseSelector->count() != static_cast<int>(warehouses.size())) {
        m_warehouseSelector->blockSignals(true);
        m_warehouseSelector->clear();
        for (const auto& [wid, wh] : warehouses) {
            m_warehouseSelector->addItem(QString("Warehouse #%1").arg(wid), wid);
        }
        m_warehouseSelector->blockSignals(false);
    }
    
    // Update current warehouse display
    if (!warehouses.empty()) {
        int currentWid = m_warehouseSelector->currentData().toInt();
        if (warehouses.find(currentWid) != warehouses.end()) {
            updateInventoryTable(warehouses.at(currentWid));
        }
    }
}

void WarehousePanel::onWarehouseSelected(int index) {
    if (m_warehousesPtr == nullptr || index < 0) return;
    
    int wid = m_warehouseSelector->itemData(index).toInt();
    auto it = m_warehousesPtr->find(wid);
    if (it != m_warehousesPtr->end()) {
        updateInventoryTable(it->second);
    }
}

void WarehousePanel::updateInventoryTable(const Warehouse& warehouse) {
    m_inventoryTable->setRowCount(0);
    
    const auto& inventory = warehouse.getAllInventory();
    int totalItems = 0;
    
    for (const auto& [itemId, qty] : inventory) {
        int row = m_inventoryTable->rowCount();
        m_inventoryTable->insertRow(row);
        
        m_inventoryTable->setItem(row, 0, new QTableWidgetItem(QString::number(itemId)));
        m_inventoryTable->setItem(row, 1, new QTableWidgetItem(QString::number(qty)));
        
        QString status;
        QColor bgColor;
        if (qty == 0) {
            status = "❌ Out of Stock";
            bgColor = QColor(255, 100, 100, 100);
        } else if (qty < 10) {
            status = "⚠️ Low Stock";
            bgColor = QColor(255, 200, 100, 100);
        } else {
            status = "✅ In Stock";
            bgColor = QColor(100, 255, 100, 100);
        }
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(status);
        statusItem->setBackground(bgColor);
        m_inventoryTable->setItem(row, 2, statusItem);
        
        totalItems += qty;
    }
    
    m_summaryLabel->setText(QString("Total Items in Warehouse #%1: %2")
        .arg(warehouse.getId()).arg(totalItems));
}

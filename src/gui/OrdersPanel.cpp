#include "OrdersPanel.h"
#include <QHeaderView>
#include <QMessageBox>

OrdersPanel::OrdersPanel(QWidget* parent)
    : QWidget(parent), m_nextOrderId(1000) {
    
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    
    // Left side: Tables
    QWidget* tablesWidget = new QWidget();
    QVBoxLayout* tablesLayout = new QVBoxLayout(tablesWidget);
    
    QSplitter* tableSplitter = new QSplitter(Qt::Vertical);
    
    // VIP Orders section
    QWidget* vipWidget = new QWidget();
    QVBoxLayout* vipLayout = new QVBoxLayout(vipWidget);
    vipLayout->setContentsMargins(5, 5, 5, 5);
    
    m_vipLabel = new QLabel("⭐ VIP Orders Queue (0)");
    QFont headerFont = m_vipLabel->font();
    headerFont.setPointSize(12);
    headerFont.setBold(true);
    m_vipLabel->setFont(headerFont);
    vipLayout->addWidget(m_vipLabel);
    
    m_vipTable = new QTableWidget();
    setupTable(m_vipTable);
    vipLayout->addWidget(m_vipTable);
    
    tableSplitter->addWidget(vipWidget);
    
    // Standard Orders section
    QWidget* stdWidget = new QWidget();
    QVBoxLayout* stdLayout = new QVBoxLayout(stdWidget);
    stdLayout->setContentsMargins(5, 5, 5, 5);
    
    m_stdLabel = new QLabel("📦 Standard Orders Queue (0)");
    m_stdLabel->setFont(headerFont);
    stdLayout->addWidget(m_stdLabel);
    
    m_stdTable = new QTableWidget();
    setupTable(m_stdTable);
    stdLayout->addWidget(m_stdTable);
    
    tableSplitter->addWidget(stdWidget);
    tablesLayout->addWidget(tableSplitter);
    
    mainLayout->addWidget(tablesWidget, 2);
    
    // Right side: Input form
    setupInputForm();
    mainLayout->addWidget(m_inputGroup, 1);
}

void OrdersPanel::setupTable(QTableWidget* table) {
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Request Time", "Due By", "Destination", "Items", "Value", "Status"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
}

void OrdersPanel::setupInputForm() {
    m_inputGroup = new QGroupBox("➕ Add New Order");
    m_inputGroup->setMinimumWidth(280);
    
    QVBoxLayout* formLayout = new QVBoxLayout(m_inputGroup);
    formLayout->setSpacing(12);
    
    // Order ID
    QHBoxLayout* idLayout = new QHBoxLayout();
    idLayout->addWidget(new QLabel("Order ID:"));
    m_orderIdSpin = new QSpinBox();
    m_orderIdSpin->setRange(1, 99999);
    m_orderIdSpin->setValue(m_nextOrderId);
    idLayout->addWidget(m_orderIdSpin);
    formLayout->addLayout(idLayout);
    
    // Destination
    QHBoxLayout* destLayout = new QHBoxLayout();
    destLayout->addWidget(new QLabel("Destination:"));
    m_destSpin = new QSpinBox();
    m_destSpin->setRange(1, 100);
    m_destSpin->setValue(1);
    destLayout->addWidget(m_destSpin);
    formLayout->addLayout(destLayout);
    
    // Due By
    QHBoxLayout* dueLayout = new QHBoxLayout();
    dueLayout->addWidget(new QLabel("Due By (0=no deadline):"));
    m_dueBySpin = new QSpinBox();
    m_dueBySpin->setRange(0, 9999);
    m_dueBySpin->setValue(50);
    dueLayout->addWidget(m_dueBySpin);
    formLayout->addLayout(dueLayout);
    
    // VIP checkbox
    m_vipCheck = new QCheckBox("⭐ VIP Order");
    formLayout->addWidget(m_vipCheck);
    
    // Separator
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    formLayout->addWidget(line1);
    
    // Items section
    QLabel* itemsLabel = new QLabel("📋 Order Items:");
    QFont itemsFont = itemsLabel->font();
    itemsFont.setBold(true);
    itemsLabel->setFont(itemsFont);
    formLayout->addWidget(itemsLabel);
    
    // Item input row
    QHBoxLayout* itemInputLayout = new QHBoxLayout();
    itemInputLayout->addWidget(new QLabel("Item:"));
    m_itemIdSpin = new QSpinBox();
    m_itemIdSpin->setRange(1, 100);
    m_itemIdSpin->setValue(1);
    m_itemIdSpin->setFixedWidth(60);
    itemInputLayout->addWidget(m_itemIdSpin);
    
    itemInputLayout->addWidget(new QLabel("Qty:"));
    m_itemQtySpin = new QSpinBox();
    m_itemQtySpin->setRange(1, 1000);
    m_itemQtySpin->setValue(10);
    m_itemQtySpin->setFixedWidth(60);
    itemInputLayout->addWidget(m_itemQtySpin);
    
    m_addItemBtn = new QPushButton("+");
    m_addItemBtn->setFixedWidth(30);
    connect(m_addItemBtn, &QPushButton::clicked, this, &OrdersPanel::onAddItemClicked);
    itemInputLayout->addWidget(m_addItemBtn);
    
    m_removeItemBtn = new QPushButton("-");
    m_removeItemBtn->setFixedWidth(30);
    connect(m_removeItemBtn, &QPushButton::clicked, this, &OrdersPanel::onRemoveItemClicked);
    itemInputLayout->addWidget(m_removeItemBtn);
    
    formLayout->addLayout(itemInputLayout);
    
    // Items table
    m_itemsTable = new QTableWidget();
    m_itemsTable->setColumnCount(2);
    m_itemsTable->setHorizontalHeaderLabels({"Item ID", "Quantity"});
    m_itemsTable->horizontalHeader()->setStretchLastSection(true);
    m_itemsTable->setMaximumHeight(120);
    m_itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTable->verticalHeader()->setVisible(false);
    formLayout->addWidget(m_itemsTable);
    
    // Separator
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    formLayout->addWidget(line2);
    
    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    m_addOrderBtn = new QPushButton("➕ Add Order");
    connect(m_addOrderBtn, &QPushButton::clicked, this, &OrdersPanel::onAddOrderClicked);
    btnLayout->addWidget(m_addOrderBtn);
    
    m_removeOrderBtn = new QPushButton("➖ Remove Selected");
    connect(m_removeOrderBtn, &QPushButton::clicked, this, &OrdersPanel::onRemoveOrderClicked);
    btnLayout->addWidget(m_removeOrderBtn);
    
    formLayout->addLayout(btnLayout);
    formLayout->addStretch();
}

void OrdersPanel::update(const std::map<int, Order>& orders,
                         const std::vector<int>& vipQueue,
                         const std::vector<int>& stdQueue) {
    
    m_vipLabel->setText(QString("⭐ VIP Orders Queue (%1)").arg(vipQueue.size()));
    m_stdLabel->setText(QString("📦 Standard Orders Queue (%1)").arg(stdQueue.size()));
    
    populateTable(m_vipTable, vipQueue, orders);
    populateTable(m_stdTable, stdQueue, orders);
}

void OrdersPanel::populateTable(QTableWidget* table, const std::vector<int>& orderIds,
                                 const std::map<int, Order>& orders) {
    table->setRowCount(0);
    
    for (int oid : orderIds) {
        auto it = orders.find(oid);
        if (it == orders.end()) continue;
        
        const Order& order = it->second;
        int row = table->rowCount();
        table->insertRow(row);
        
        table->setItem(row, 0, new QTableWidgetItem(QString::number(order.getId())));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(order.getRequestTime())));
        table->setItem(row, 2, new QTableWidgetItem(order.getDueBy() > 0 ? QString::number(order.getDueBy()) : "N/A"));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(order.getDestination())));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(order.getTotalQuantity())));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(order.getValue(), 'f', 1)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(order.getStatusString()));
        statusItem->setBackground(getStatusColor(order.getStatus()));
        table->setItem(row, 6, statusItem);
    }
}

QColor OrdersPanel::getStatusColor(OrderStatus status) {
    switch (status) {
        case OrderStatus::Waiting: return QColor(255, 200, 100, 100);
        case OrderStatus::Assigned: return QColor(100, 150, 255, 100);
        case OrderStatus::InTransit: return QColor(100, 200, 255, 100);
        case OrderStatus::Delivered: return QColor(100, 255, 100, 100);
        case OrderStatus::Canceled: return QColor(150, 150, 150, 100);
        default: return QColor(50, 50, 50, 100);
    }
}

void OrdersPanel::onAddItemClicked() {
    int itemId = m_itemIdSpin->value();
    int qty = m_itemQtySpin->value();
    
    // Check if item already exists
    for (auto& item : m_currentItems) {
        if (item.first == itemId) {
            item.second += qty;  // Add to existing
            updateItemsList();
            return;
        }
    }
    
    m_currentItems.push_back({itemId, qty});
    updateItemsList();
}

void OrdersPanel::onRemoveItemClicked() {
    int currentRow = m_itemsTable->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(m_currentItems.size())) {
        m_currentItems.erase(m_currentItems.begin() + currentRow);
        updateItemsList();
    }
}

void OrdersPanel::updateItemsList() {
    m_itemsTable->setRowCount(0);
    for (const auto& item : m_currentItems) {
        int row = m_itemsTable->rowCount();
        m_itemsTable->insertRow(row);
        m_itemsTable->setItem(row, 0, new QTableWidgetItem(QString::number(item.first)));
        m_itemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.second)));
    }
}

void OrdersPanel::onAddOrderClicked() {
    if (m_currentItems.empty()) {
        QMessageBox::warning(this, "Error", "Please add at least one item to the order!");
        return;
    }
    
    int orderId = m_orderIdSpin->value();
    int dest = m_destSpin->value();
    int dueBy = m_dueBySpin->value();
    bool isVip = m_vipCheck->isChecked();
    
    emit addOrderRequested(orderId, dest, dueBy, isVip, m_currentItems);
    
    // Reset form
    m_nextOrderId = orderId + 1;
    m_orderIdSpin->setValue(m_nextOrderId);
    m_currentItems.clear();
    updateItemsList();
    m_vipCheck->setChecked(false);
}

void OrdersPanel::onRemoveOrderClicked() {
    // Check VIP table first
    int row = m_vipTable->currentRow();
    if (row >= 0) {
        int orderId = m_vipTable->item(row, 0)->text().toInt();
        emit removeOrderRequested(orderId);
        return;
    }
    
    // Check Standard table
    row = m_stdTable->currentRow();
    if (row >= 0) {
        int orderId = m_stdTable->item(row, 0)->text().toInt();
        emit removeOrderRequested(orderId);
        return;
    }
    
    QMessageBox::information(this, "Info", "Please select an order from the table to remove.");
}

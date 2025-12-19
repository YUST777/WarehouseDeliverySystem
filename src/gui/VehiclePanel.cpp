#include "VehiclePanel.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGroupBox>

VehiclePanel::VehiclePanel(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel* titleLabel = new QLabel("🚚 Vehicle Fleet");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }");
    
    m_scrollContent = new QWidget();
    m_scrollContent->setStyleSheet("background: transparent;");
    m_gridLayout = new QGridLayout(m_scrollContent);
    m_gridLayout->setSpacing(15);
    m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    
    scrollArea->setWidget(m_scrollContent);
    mainLayout->addWidget(scrollArea);
}

void VehiclePanel::update(const std::map<int, Vehicle>& vehicles) {
    // Clear existing cards
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // Create new cards
    int col = 0, row = 0;
    const int maxCols = 4;
    
    for (const auto& [vid, vehicle] : vehicles) {
        QWidget* card = createVehicleCard(vehicle);
        m_gridLayout->addWidget(card, row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

QWidget* VehiclePanel::createVehicleCard(const Vehicle& vehicle) {
    QGroupBox* card = new QGroupBox(QString("%1 Vehicle #%2")
        .arg(vehicle.isRefrigerated() ? "❄️" : "📦")
        .arg(vehicle.getId()));
        
    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setSpacing(5);
    
    // Status
    QString statusIcon = getStatusIcon(vehicle.getStatus());
    QLabel* status = new QLabel(QString("%1 %2")
        .arg(statusIcon)
        .arg(QString::fromStdString(vehicle.getStatusString())));
        
    // Use standard palette for text colors if possible, or muted colors
    // We'll keep some color for status to make it readable but avoid the heavy styling
    QPalette pal = status->palette();
    if (vehicle.getStatus() == VehicleStatus::Available) pal.setColor(QPalette::WindowText, Qt::darkGreen);
    else if (vehicle.getStatus() == VehicleStatus::Maintenance) pal.setColor(QPalette::WindowText, Qt::darkRed);
    else if (vehicle.getStatus() == VehicleStatus::Returning) pal.setColor(QPalette::WindowText, QColor(255, 140, 0)); // Dark Orange
    status->setPalette(pal);
    
    layout->addWidget(status);
    
    // Details
    layout->addWidget(new QLabel(QString("Type: %1").arg(QString::fromStdString(vehicle.getTypeString()))));
    
    QLabel* capacity = new QLabel(QString("Capacity: %1 | Speed: %2")
        .arg(vehicle.getCapacity()).arg(vehicle.getSpeed()));
    layout->addWidget(capacity);
    
    QLabel* home = new QLabel(QString("Home: Warehouse #%1").arg(vehicle.getHomeWarehouse()));
    layout->addWidget(home);
    
    // Orders (if any)
    if (!vehicle.getAssignedOrders().empty()) {
        QString orderStr = "Orders: ";
        for (int oid : vehicle.getAssignedOrders()) {
            orderStr += QString("#%1 ").arg(oid);
        }
        QLabel* orders = new QLabel(orderStr);
        // orders->setStyleSheet("font-weight: bold;"); // Optional bold
        layout->addWidget(orders);
    }
    
    layout->addStretch();
    return card;
}

QColor VehiclePanel::getStatusColor(VehicleStatus status) {
    switch (status) {
        case VehicleStatus::Available: return QColor("#4ade80");
        case VehicleStatus::Outbound: return QColor("#60a5fa");
        case VehicleStatus::Returning: return QColor("#fb923c");
        case VehicleStatus::Maintenance: return QColor("#f87171");
        default: return QColor("#9ca3af");
    }
}

QString VehiclePanel::getStatusIcon(VehicleStatus status) {
    switch (status) {
        case VehicleStatus::Available: return "✅";
        case VehicleStatus::Outbound: return "🚚";
        case VehicleStatus::Returning: return "↩️";
        case VehicleStatus::Maintenance: return "🔧";
        default: return "❓";
    }
}

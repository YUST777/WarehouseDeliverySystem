#include "VehiclePanel.h"
#include <QScrollArea>
#include <QVBoxLayout>

VehiclePanel::VehiclePanel(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel* titleLabel = new QLabel("🚚 Vehicle Fleet");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560;");
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
    QFrame* card = new QFrame();
    card->setFixedSize(200, 150);
    card->setFrameShape(QFrame::StyledPanel);
    
    QColor statusColor = getStatusColor(vehicle.getStatus());
    card->setStyleSheet(QString(
        "QFrame { background-color: #16213e; border: 2px solid %1; border-radius: 10px; }"
    ).arg(statusColor.name()));
    
    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    
    // Header with ID and type
    QLabel* header = new QLabel(QString("%1 Vehicle #%2")
        .arg(vehicle.isRefrigerated() ? "❄️" : "📦")
        .arg(vehicle.getId()));
    header->setStyleSheet("font-size: 14px; font-weight: bold; color: #e0e0e0;");
    layout->addWidget(header);
    
    // Status
    QString statusIcon = getStatusIcon(vehicle.getStatus());
    QLabel* status = new QLabel(QString("%1 %2")
        .arg(statusIcon)
        .arg(QString::fromStdString(vehicle.getStatusString())));
    status->setStyleSheet(QString("font-size: 13px; color: %1; font-weight: bold;").arg(statusColor.name()));
    layout->addWidget(status);
    
    // Details
    QLabel* type = new QLabel(QString("Type: %1").arg(QString::fromStdString(vehicle.getTypeString())));
    type->setStyleSheet("font-size: 11px; color: #87ceeb;");
    layout->addWidget(type);
    
    QLabel* capacity = new QLabel(QString("Capacity: %1 | Speed: %2")
        .arg(vehicle.getCapacity()).arg(vehicle.getSpeed()));
    capacity->setStyleSheet("font-size: 11px; color: #87ceeb;");
    layout->addWidget(capacity);
    
    QLabel* home = new QLabel(QString("Home: Warehouse #%1").arg(vehicle.getHomeWarehouse()));
    home->setStyleSheet("font-size: 11px; color: #87ceeb;");
    layout->addWidget(home);
    
    // Orders (if any)
    if (!vehicle.getAssignedOrders().empty()) {
        QString orderStr = "Orders: ";
        for (int oid : vehicle.getAssignedOrders()) {
            orderStr += QString("#%1 ").arg(oid);
        }
        QLabel* orders = new QLabel(orderStr);
        orders->setStyleSheet("font-size: 10px; color: #ffd700;");
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

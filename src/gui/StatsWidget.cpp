#include "StatsWidget.h"
#include <QGridLayout>
#include <QFrame>

StatsWidget::StatsWidget(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("📊 Simulation Statistics");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #e94560;");
    mainLayout->addWidget(titleLabel);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    
    auto createCard = [](const QString& title, const QString& icon, QLabel** valueLabel) {
        QFrame* card = new QFrame();
        card->setStyleSheet(
            "QFrame { background-color: #16213e; border-radius: 10px; padding: 15px; }"
        );
        
        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        
        QLabel* titleLbl = new QLabel(QString("%1 %2").arg(icon).arg(title));
        titleLbl->setStyleSheet("font-size: 12px; color: #87ceeb;");
        cardLayout->addWidget(titleLbl);
        
        *valueLabel = new QLabel("0");
        (*valueLabel)->setStyleSheet("font-size: 28px; font-weight: bold; color: #e0e0e0;");
        cardLayout->addWidget(*valueLabel);
        
        return card;
    };
    
    gridLayout->addWidget(createCard("Total Orders", "📦", &m_totalOrdersValue), 0, 0);
    gridLayout->addWidget(createCard("Delivered", "✅", &m_deliveredValue), 0, 1);
    gridLayout->addWidget(createCard("Canceled", "❌", &m_canceledValue), 0, 2);
    gridLayout->addWidget(createCard("VIP Orders", "⭐", &m_vipValue), 1, 0);
    gridLayout->addWidget(createCard("Standard", "📋", &m_stdValue), 1, 1);
    gridLayout->addWidget(createCard("Total Value", "💰", &m_totalValueLabel), 1, 2);
    gridLayout->addWidget(createCard("Avg Wait Time", "⏱️", &m_avgWaitValue), 2, 0);
    gridLayout->addWidget(createCard("Avg Transit", "🚚", &m_avgTransitValue), 2, 1);
    gridLayout->addWidget(createCard("On-Time Rate", "🎯", &m_onTimeValue), 2, 2);
    
    mainLayout->addLayout(gridLayout);
    
    // Delivery progress bar
    QFrame* progressFrame = new QFrame();
    progressFrame->setStyleSheet("QFrame { background-color: #16213e; border-radius: 10px; padding: 15px; }");
    QVBoxLayout* progressLayout = new QVBoxLayout(progressFrame);
    
    QLabel* progressTitle = new QLabel("📈 Delivery Progress");
    progressTitle->setStyleSheet("font-size: 14px; color: #87ceeb;");
    progressLayout->addWidget(progressTitle);
    
    m_deliveryProgress = new QProgressBar();
    m_deliveryProgress->setRange(0, 100);
    m_deliveryProgress->setValue(0);
    m_deliveryProgress->setTextVisible(true);
    m_deliveryProgress->setStyleSheet(
        "QProgressBar { border: none; background-color: #0a0a1a; border-radius: 8px; height: 20px; text-align: center; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e94560, stop:1 #0f3460); border-radius: 8px; }"
    );
    progressLayout->addWidget(m_deliveryProgress);
    
    mainLayout->addWidget(progressFrame);
    mainLayout->addStretch();
}

void StatsWidget::update(const Simulator::Statistics& stats) {
    m_totalOrdersValue->setText(QString::number(stats.totalOrders));
    m_deliveredValue->setText(QString::number(stats.deliveredOrders));
    m_canceledValue->setText(QString::number(stats.canceledOrders));
    m_vipValue->setText(QString::number(stats.vipOrders));
    m_stdValue->setText(QString::number(stats.stdOrders));
    m_totalValueLabel->setText(QString::number(stats.totalValue, 'f', 0));
    m_avgWaitValue->setText(QString::number(stats.avgWaitTime, 'f', 1));
    m_avgTransitValue->setText(QString::number(stats.avgTransitTime, 'f', 1));
    m_onTimeValue->setText(QString("%1%").arg(stats.onTimeRate, 0, 'f', 1));
    
    int progress = stats.totalOrders > 0 
        ? (stats.deliveredOrders * 100 / stats.totalOrders) : 0;
    m_deliveryProgress->setValue(progress);
}

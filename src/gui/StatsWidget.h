#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include "core/Simulator.h"

class StatsWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit StatsWidget(QWidget* parent = nullptr);
    
    void update(const Simulator::Statistics& stats);
    
private:
    QWidget* createStatCard(const QString& title, const QString& icon);
    
    QLabel* m_totalOrdersValue;
    QLabel* m_deliveredValue;
    QLabel* m_canceledValue;
    QLabel* m_vipValue;
    QLabel* m_stdValue;
    QLabel* m_totalValueLabel;
    QLabel* m_avgWaitValue;
    QLabel* m_avgTransitValue;
    QLabel* m_onTimeValue;
    QProgressBar* m_deliveryProgress;
};

#endif // STATSWIDGET_H

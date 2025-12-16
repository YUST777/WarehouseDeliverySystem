#ifndef VEHICLEPANEL_H
#define VEHICLEPANEL_H

#include <QWidget>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <map>
#include "models/Vehicle.h"

class VehiclePanel : public QWidget {
    Q_OBJECT
    
public:
    explicit VehiclePanel(QWidget* parent = nullptr);
    
    void update(const std::map<int, Vehicle>& vehicles);
    
private:
    QWidget* createVehicleCard(const Vehicle& vehicle);
    QColor getStatusColor(VehicleStatus status);
    QString getStatusIcon(VehicleStatus status);
    
    QGridLayout* m_gridLayout;
    QWidget* m_scrollContent;
};

#endif // VEHICLEPANEL_H

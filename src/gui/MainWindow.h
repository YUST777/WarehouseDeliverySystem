#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QSplitter>
#include <QLabel>
#include "core/Simulator.h"
#include "OrdersPanel.h"
#include "WarehousePanel.h"
#include "VehiclePanel.h"
#include "MapWidget.h"
#include "EventLogWidget.h"
#include "ControlBar.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void onOpenFile();
    void onSaveResults();
    void onStep();
    void onRun();
    void onPause();
    void onReset();
    void onSpeedChanged(int speed);
    void onTimeAdvanced(int time);
    void onSimulationFinished();
    void onLogMessage(const QString& message);
    void onAddOrder(int orderId, int dest, int dueBy, bool isVip, 
                    const std::vector<std::pair<int, int>>& items);
    void onRemoveOrder(int orderId);
    void updateAllPanels();
    
private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupConnections();
    
    // Core simulator
    Simulator* m_simulator;
    
    // Menu & Toolbar
    QMenuBar* m_menuBar;
    QToolBar* m_toolBar;
    ControlBar* m_controlBar;
    
    // Main panels
    QTabWidget* m_tabWidget;
    OrdersPanel* m_ordersPanel;
    WarehousePanel* m_warehousePanel;
    VehiclePanel* m_vehiclePanel;
    MapWidget* m_mapWidget;
    
    // Side panel
    EventLogWidget* m_eventLog;
    
    // Status bar labels
    QLabel* m_timeLabel;
    QLabel* m_statusLabel;
};

#endif // MAINWINDOW_H

#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    
    setWindowTitle("Warehouse & Delivery Management System");
    setMinimumSize(1000, 700); // Allow resizing to smaller screens
    
    // Create simulator
    m_simulator = new Simulator(this);
    
    // Setup UI components
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupConnections();
    
    // Initial status
    m_timeLabel = new QLabel("Time: 0");
    m_statusLabel = new QLabel("Ready - Add orders manually or load a file");
    statusBar()->addWidget(m_timeLabel);
    statusBar()->addPermanentWidget(m_statusLabel);
}

MainWindow::~MainWindow() {}

void MainWindow::setupMenuBar() {
    m_menuBar = menuBar();
    
    // File menu
    QMenu* fileMenu = m_menuBar->addMenu("&File");
    
    QAction* openAction = new QAction("&Open Input File...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    fileMenu->addAction(openAction);
    
    QAction* saveAction = new QAction("&Save Results...", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveResults);
    fileMenu->addAction(saveAction);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // Simulation menu
    QMenu* simMenu = m_menuBar->addMenu("&Simulation");
    
    QAction* stepAction = new QAction("&Step", this);
    stepAction->setShortcut(Qt::Key_Space);
    connect(stepAction, &QAction::triggered, this, &MainWindow::onStep);
    simMenu->addAction(stepAction);
    
    QAction* runAction = new QAction("&Run", this);
    runAction->setShortcut(Qt::Key_F5);
    connect(runAction, &QAction::triggered, this, &MainWindow::onRun);
    simMenu->addAction(runAction);
    
    QAction* pauseAction = new QAction("&Pause", this);
    pauseAction->setShortcut(Qt::Key_F6);
    connect(pauseAction, &QAction::triggered, this, &MainWindow::onPause);
    simMenu->addAction(pauseAction);
    
    simMenu->addSeparator();
    
    QAction* resetAction = new QAction("R&eset", this);
    resetAction->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(resetAction, &QAction::triggered, this, &MainWindow::onReset);
    simMenu->addAction(resetAction);
    
    // Help menu
    QMenu* helpMenu = m_menuBar->addMenu("&Help");
    
    QAction* aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About", 
            "Warehouse & Delivery Management System\n\n"
            "A discrete event simulation for warehouse operations.\n\n"
            "Data Structures & Algorithms Project");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupToolBar() {
    m_toolBar = addToolBar("Main Toolbar");
    m_toolBar->setMovable(false);
    
    m_controlBar = new ControlBar(this);
    m_toolBar->addWidget(m_controlBar);
    
    connect(m_controlBar, &ControlBar::stepClicked, this, &MainWindow::onStep);
    connect(m_controlBar, &ControlBar::runClicked, this, &MainWindow::onRun);
    connect(m_controlBar, &ControlBar::pauseClicked, this, &MainWindow::onPause);
    connect(m_controlBar, &ControlBar::resetClicked, this, &MainWindow::onReset);
    connect(m_controlBar, &ControlBar::speedChanged, this, &MainWindow::onSpeedChanged);
}

void MainWindow::setupCentralWidget() {
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left side: Tab widget with panels
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    
    // Tab widget for main panels
    m_tabWidget = new QTabWidget();
    
    m_ordersPanel = new OrdersPanel(this);
    m_warehousePanel = new WarehousePanel(this);
    m_vehiclePanel = new VehiclePanel(this);
    
    m_tabWidget->addTab(m_ordersPanel, "📦 Orders");
    m_tabWidget->addTab(m_warehousePanel, "🏭 Warehouses");
    m_tabWidget->addTab(m_vehiclePanel, "🚚 Vehicles");
    
    splitter->addWidget(m_tabWidget);
    
    // Right side: Event log
    m_eventLog = new EventLogWidget(this);
    splitter->addWidget(m_eventLog);
    
    splitter->setSizes({800, 400});
    
    mainLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections() {
    connect(m_simulator, &Simulator::timeAdvanced, this, &MainWindow::onTimeAdvanced);
    connect(m_simulator, &Simulator::simulationFinished, this, &MainWindow::onSimulationFinished);
    connect(m_simulator, &Simulator::logMessage, this, &MainWindow::onLogMessage);
    
    connect(m_simulator, &Simulator::orderArrived, this, &MainWindow::updateAllPanels);
    connect(m_simulator, &Simulator::orderDelivered, this, &MainWindow::updateAllPanels);
    connect(m_simulator, &Simulator::orderCanceled, this, &MainWindow::updateAllPanels);
    connect(m_simulator, &Simulator::vehicleDispatched, this, &MainWindow::updateAllPanels);
    connect(m_simulator, &Simulator::inventoryRestocked, this, &MainWindow::updateAllPanels);
    
    // Connect order panel signals
    connect(m_ordersPanel, &OrdersPanel::addOrderRequested, this, &MainWindow::onAddOrder);
    connect(m_ordersPanel, &OrdersPanel::removeOrderRequested, this, &MainWindow::onRemoveOrder);
}

void MainWindow::onOpenFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Input File", "", 
        "Text Files (*.txt);;All Files (*)");
    
    if (!filename.isEmpty()) {
        if (m_simulator->loadFromFile(filename)) {
            m_statusLabel->setText("Loaded: " + filename);
            updateAllPanels();
            m_eventLog->clear();
            m_eventLog->addMessage("Simulation loaded from: " + filename);
        } else {
            QMessageBox::critical(this, "Error", "Failed to load file");
        }
    }
}

void MainWindow::onSaveResults() {
    QString filename = QFileDialog::getSaveFileName(this, "Save Results", "output.txt",
        "Text Files (*.txt);;All Files (*)");
    
    if (!filename.isEmpty()) {
        if (m_simulator->saveResults(filename)) {
            m_statusLabel->setText("Results saved to: " + filename);
        } else {
            QMessageBox::critical(this, "Error", "Failed to save results");
        }
    }
}

void MainWindow::onStep() {
    m_simulator->step();
    updateAllPanels();
}

void MainWindow::onRun() {
    m_simulator->run();
    m_controlBar->setRunning(true);
    m_statusLabel->setText("Simulation running...");
}

void MainWindow::onPause() {
    m_simulator->pause();
    m_controlBar->setRunning(false);
    m_statusLabel->setText("Simulation paused");
}

void MainWindow::onReset() {
    m_simulator->reset();
    m_controlBar->setRunning(false);
    updateAllPanels();
    m_eventLog->clear();
    m_statusLabel->setText("Simulation reset");
}

void MainWindow::onSpeedChanged(int speed) {
    m_simulator->setSpeed(speed);
}

void MainWindow::onTimeAdvanced(int time) {
    m_timeLabel->setText(QString("Time: %1").arg(time));
    updateAllPanels();
}

void MainWindow::onSimulationFinished() {
    m_controlBar->setRunning(false);
    m_statusLabel->setText("Simulation finished!");
    QMessageBox::information(this, "Complete", "Simulation has finished!");
}

void MainWindow::onLogMessage(const QString& message) {
    m_eventLog->addMessage(message);
}

void MainWindow::onAddOrder(int orderId, int dest, int dueBy, bool isVip,
                            const std::vector<std::pair<int, int>>& items) {
    m_simulator->addManualOrder(orderId, dest, dueBy, isVip, items);
    updateAllPanels();
    m_eventLog->addMessage(QString("Manual order #%1 added (%2)")
        .arg(orderId).arg(isVip ? "VIP" : "Standard"));
    m_statusLabel->setText(QString("Added order #%1").arg(orderId));
}

void MainWindow::onRemoveOrder(int orderId) {
    m_simulator->cancelOrder(orderId);
    updateAllPanels();
    m_eventLog->addMessage(QString("Order #%1 removed").arg(orderId));
}

void MainWindow::updateAllPanels() {
    m_ordersPanel->update(m_simulator->getOrders(), 
                          m_simulator->getVipQueue(),
                          m_simulator->getStdQueue());
    m_warehousePanel->update(m_simulator->getWarehouses());
    m_vehiclePanel->update(m_simulator->getVehicles());
}

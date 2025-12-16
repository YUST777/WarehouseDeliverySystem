#include "ControlBar.h"

ControlBar::ControlBar(QWidget* parent)
    : QWidget(parent) {
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(15);
    
    // Step button
    m_stepBtn = new QPushButton("⏭ Step");
    m_stepBtn->setToolTip("Advance one timestep (Space)");
    m_stepBtn->setMinimumWidth(80);
    connect(m_stepBtn, &QPushButton::clicked, this, &ControlBar::stepClicked);
    layout->addWidget(m_stepBtn);
    
    // Run button
    m_runBtn = new QPushButton("▶ Run");
    m_runBtn->setToolTip("Start continuous simulation (F5)");
    m_runBtn->setMinimumWidth(80);
    connect(m_runBtn, &QPushButton::clicked, this, &ControlBar::runClicked);
    layout->addWidget(m_runBtn);
    
    // Pause button
    m_pauseBtn = new QPushButton("⏸ Pause");
    m_pauseBtn->setToolTip("Pause simulation (F6)");
    m_pauseBtn->setMinimumWidth(80);
    m_pauseBtn->setEnabled(false);
    connect(m_pauseBtn, &QPushButton::clicked, this, &ControlBar::pauseClicked);
    layout->addWidget(m_pauseBtn);
    
    // Reset button
    m_resetBtn = new QPushButton("⏹ Reset");
    m_resetBtn->setToolTip("Reset simulation (Ctrl+R)");
    m_resetBtn->setMinimumWidth(80);
    connect(m_resetBtn, &QPushButton::clicked, this, &ControlBar::resetClicked);
    layout->addWidget(m_resetBtn);
    
    layout->addSpacing(30);
    
    // Speed control
    QLabel* speedTitle = new QLabel("Speed:");
    layout->addWidget(speedTitle);
    
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(50, 2000);
    m_speedSlider->setValue(500);
    m_speedSlider->setFixedWidth(150);
    m_speedSlider->setToolTip("Simulation speed (ms per step)");
    connect(m_speedSlider, &QSlider::valueChanged, [this](int value) {
        m_speedLabel->setText(QString("%1 ms").arg(value));
        emit speedChanged(value);
    });
    layout->addWidget(m_speedSlider);
    
    m_speedLabel = new QLabel("500 ms");
    m_speedLabel->setMinimumWidth(60);
    layout->addWidget(m_speedLabel);
    
    layout->addStretch();
}

void ControlBar::setRunning(bool running) {
    m_stepBtn->setEnabled(!running);
    m_runBtn->setEnabled(!running);
    m_pauseBtn->setEnabled(running);
    m_resetBtn->setEnabled(!running);
}

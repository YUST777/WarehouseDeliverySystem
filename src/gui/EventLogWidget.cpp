#include "EventLogWidget.h"
#include <QScrollBar>
#include <QDateTime>

EventLogWidget::EventLogWidget(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    QLabel* titleLabel = new QLabel("📜 Event Log");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #e94560; padding: 5px;");
    layout->addWidget(titleLabel);
    
    m_logText = new QTextEdit();
    m_logText->setReadOnly(true);
    m_logText->setStyleSheet(
        "QTextEdit { background-color: #0a0a1a; color: #00ff00; "
        "font-family: 'Consolas', 'Monaco', monospace; font-size: 11px; "
        "border: 1px solid #3a3a5a; padding: 5px; }"
    );
    layout->addWidget(m_logText);
}

void EventLogWidget::addMessage(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedMsg = QString("<span style='color: #888888;'>[%1]</span> %2").arg(timestamp).arg(message);
    
    m_logText->append(formattedMsg);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = m_logText->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void EventLogWidget::clear() {
    m_logText->clear();
    addMessage("Event log cleared");
}

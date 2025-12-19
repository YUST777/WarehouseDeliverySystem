#include "EventLogWidget.h"
#include <QScrollBar>
#include <QDateTime>

EventLogWidget::EventLogWidget(QWidget* parent)
    : QWidget(parent) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    QLabel* titleLabel = new QLabel("📜 Event Log");
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    
    m_logText = new QTextEdit();
    m_logText->setReadOnly(true);
    m_logText->setFont(QFont("Monospace"));
    layout->addWidget(m_logText);
}

void EventLogWidget::addMessage(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    // Use gray for timestamp, standard text color for message
    QString formattedMsg = QString("<span style='color: gray;'>[%1]</span> %2").arg(timestamp).arg(message);
    
    m_logText->append(formattedMsg);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = m_logText->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void EventLogWidget::clear() {
    m_logText->clear();
    addMessage("Event log cleared");
}

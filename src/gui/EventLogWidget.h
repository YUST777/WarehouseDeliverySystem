#ifndef EVENTLOGWIDGET_H
#define EVENTLOGWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>

class EventLogWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit EventLogWidget(QWidget* parent = nullptr);
    
    void addMessage(const QString& message);
    void clear();
    
private:
    QTextEdit* m_logText;
};

#endif // EVENTLOGWIDGET_H

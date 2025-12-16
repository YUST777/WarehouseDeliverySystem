#ifndef CONTROLBAR_H
#define CONTROLBAR_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>

class ControlBar : public QWidget {
    Q_OBJECT
    
public:
    explicit ControlBar(QWidget* parent = nullptr);
    
    void setRunning(bool running);
    
signals:
    void stepClicked();
    void runClicked();
    void pauseClicked();
    void resetClicked();
    void speedChanged(int msPerStep);
    
private:
    QPushButton* m_stepBtn;
    QPushButton* m_runBtn;
    QPushButton* m_pauseBtn;
    QPushButton* m_resetBtn;
    QSlider* m_speedSlider;
    QLabel* m_speedLabel;
};

#endif // CONTROLBAR_H

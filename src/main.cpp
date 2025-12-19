#include <QApplication>
#include <QStyleFactory>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("Warehouse & Delivery Management System");
    QApplication::setOrganizationName("DSA Project");
    QApplication::setApplicationVersion("1.0");
    
    // Apply VS Code Modern Dark Styling
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    QPalette darkPalette;
    QColor darkGray(30, 30, 30);      // VS Code background #1e1e1e
    QColor lighterGray(45, 45, 48);   // VS Code sidebar/input #2d2d30
    QColor blueAccent(0, 122, 204);   // VS Code Blue #007acc
    QColor textColor(204, 204, 204);  // VS Code text #cccccc
    
    darkPalette.setColor(QPalette::Window, darkGray);
    darkPalette.setColor(QPalette::WindowText, textColor);
    darkPalette.setColor(QPalette::Base, lighterGray);
    darkPalette.setColor(QPalette::AlternateBase, darkGray);
    darkPalette.setColor(QPalette::ToolTipBase, blueAccent);
    darkPalette.setColor(QPalette::ToolTipText, textColor);
    darkPalette.setColor(QPalette::Text, textColor);
    darkPalette.setColor(QPalette::Button, darkGray);
    darkPalette.setColor(QPalette::ButtonText, textColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, blueAccent);
    darkPalette.setColor(QPalette::Highlight, blueAccent);
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    
    QApplication::setPalette(darkPalette);

    // Modern Stylesheet for specific widgets
    app.setStyleSheet(
        "QToolTip { color: #ffffff; background-color: #007acc; border: 1px solid white; }"
        "QHeaderView::section { background-color: #2d2d30; color: #cccccc; padding: 5px; border: none; }"
        "QTableWidget { gridline-color: #3e3e42; }"
        "QTableWidget::item { padding: 5px; }"
        "QScrollBar:vertical { border: none; background: #1e1e1e; width: 14px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #424242; min-height: 20px; border-radius: 7px; margin: 2px; }"
        "QScrollBar::handle:vertical:hover { background: #686868; }"
        "QPushButton { background-color: #3c3c3c; border: 1px solid #3e3e42; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #505050; border-color: #007acc; }"
        "QPushButton:pressed { background-color: #007acc; color: white; }"
        "QLineEdit, QSpinBox { background-color: #3c3c3c; border: 1px solid #3e3e42; padding: 4px; border-radius: 2px; }"
        "QLineEdit:focus, QSpinBox:focus { border: 1px solid #007acc; }"
        "QGroupBox { border: 1px solid #3e3e42; border-radius: 4px; margin-top: 10px; padding-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; left: 10px; }"
    );
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

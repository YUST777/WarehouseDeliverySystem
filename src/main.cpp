#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("Warehouse & Delivery Management System");
    QApplication::setOrganizationName("DSA Project");
    QApplication::setApplicationVersion("1.0");
    
    // Set application-wide font
    QFont font("Segoe UI", 10);
    app.setFont(font);
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("Warehouse & Delivery Management System");
    QApplication::setOrganizationName("DSA Project");
    QApplication::setApplicationVersion("1.0");
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

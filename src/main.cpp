#include "TrayApplication.h"

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>

#define EXIT_ERROR 1

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setWindowIcon(QIcon(":/icons/TimeFlip.png"));
    qApp->setOrganizationName("VanillaFairySoftware");
    qApp->setApplicationName("TimeFlipAgent");

    // Check if the system tray is supported
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Error", "System tray not supported on this system.");
        return EXIT_ERROR;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    TrayApplication trayApp;

    return a.exec();
}

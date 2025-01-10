#include "TrayApplication.h"

#include "Config.h"
#include "Configuration.h"
#include "Summary.h"

#include "TimeFlipApiClient.h"
#include "TimeFlipBTClient.h"

#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QScreen>
#include <QSystemTrayIcon>

using namespace TimeFlipApi;
using namespace TimeFlipBT;

TrayApplication::TrayApplication(QObject *parent)
    : QObject(parent)
    , m_btClient(new TimeFlipBTClient(this))
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/TimeFlip.png"), this);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &TrayApplication::handleTrayIconActivation);

    // Setup context menu
    QMenu *trayMenu = new QMenu();
    QAction *summaryAction = trayMenu->addAction("Summary");
    connect(summaryAction, &QAction::triggered, this, &TrayApplication::showSummary);
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(m_btClient, &TimeFlipBTClient::connected, this, &TrayApplication::handleConnectionToDevice);
    connect(m_btClient, &TimeFlipBTClient::disconnected, this, &TrayApplication::handleDisconnectionFromDevice);


    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    Config &config = Config::instance();
    config.load();

    m_btClient->startDiscovery();
}

void TrayApplication::handleConnectionToDevice()
{
    m_trayIcon->showMessage("TimeFlip", "Connected to a TimeFlip device", QSystemTrayIcon::Information);
}

void TrayApplication::handleDisconnectionFromDevice()
{
    m_trayIcon->showMessage("TimeFlip", "Disconnected from a TimeFlip device", QSystemTrayIcon::Information);
}

void TrayApplication::handleTrayIconActivation(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        showSummary();
        break;
    case QSystemTrayIcon::Context:
        // A bit excessive since it will get invoked anyway, but as we're processing activation anyway...
        m_trayIcon->contextMenu()->popup(QCursor::pos());
        break;
    default:
        break;
    }
}

TrayApplication::~TrayApplication() = default;

void TrayApplication::showSummary()
{
    if (m_summary == nullptr) {
        m_summary = std::make_unique<Summary>(nullptr);
        m_summary->setWindowFlags(m_summary->windowFlags() | Qt::WindowStaysOnTopHint);
    }

    if (m_summary->isHidden()) {
        m_summary->show();
        positionWidgetBottomRight(m_summary.get());
    } else {
        m_summary->hide();
    }
}

void TrayApplication::positionWidgetBottomRight(QWidget *widget)
{
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    const QRect availableGeometry = screen->availableGeometry();
    const int x = availableGeometry.right() - widget->frameGeometry().width();
    const int y = availableGeometry.bottom() - widget->frameGeometry().height();
    widget->move(x, y);
}

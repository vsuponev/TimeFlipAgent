#include "TrayApplication.h"

#include "Config.h"
#include "Configuration.h"
#include "Summary.h"

#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QScreen>
#include <QSystemTrayIcon>

TrayApplication::TrayApplication() {
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/TimeFlip.png"), this);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &TrayApplication::handleTrayIconActivation);

    // Setup context menu
    QMenu *trayMenu = new QMenu();
    QAction *summaryAction = trayMenu->addAction("Summary");
    connect(summaryAction, &QAction::triggered, this, &TrayApplication::showSummary);
    QAction *configurationAction = trayMenu->addAction("Configuration");
    connect(configurationAction, &QAction::triggered, this, &TrayApplication::showConfiguration);
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    Config &config = Config::instance();
    config.load();

    // If not configured, show configuration dialog
    if (!config.isValid()) {
        showConfiguration();
    }
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

void TrayApplication::showConfiguration()
{
    if (m_configuration == nullptr) {
        m_configuration = std::make_unique<Configuration>(nullptr);
        m_configuration->setWindowFlags(m_configuration->windowFlags() | Qt::WindowStaysOnTopHint);
        connect(m_configuration.get(), &Configuration::configurationUpdated, this, &TrayApplication::handleConfigurationUpdate);
    }

    if (m_configuration->isHidden()) {
        m_configuration->show();
    } else {
        m_configuration->hide();
    }
}

void TrayApplication::handleConfigurationUpdate()
{
    m_trayIcon->showMessage("Communication error", "Unable to connect to the TimeFlip2 server!", QSystemTrayIcon::Warning);
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

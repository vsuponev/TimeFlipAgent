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
    , m_apiClient(new TimeFlipApiClient(this))
    , m_btClient(new TimeFlipBTClient(this))
{
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

    // Setup API client
    connect(m_apiClient, &TimeFlipApiClient::error, this, [this](const QString &message) {
        m_trayIcon->showMessage("Error", message, QSystemTrayIcon::Critical);
    });
    connect(m_apiClient, &TimeFlipApiClient::authenticated, this, [this](const UserInfo &userInfo) {
        m_trayIcon->showMessage("", "Logged in as " + userInfo.fullName);
        m_apiClient->requestTasks();
    });
    connect(m_apiClient, &TimeFlipApiClient::tasksReceived, this, [this]() {
        qDebug() << "Tasks list received";
        m_btClient->startDiscovery();
    });

    connect(m_btClient, &TimeFlipBTClient::facetChanged, this, [this](int facet) {
        const auto task = m_apiClient->taskBySideIndex(facet);
        if (!task.isValid()) {
            qCritical() << "Invalid task";
            return;
        }
        qDebug().noquote().nospace() << "Active facet [" << facet << "]: " << task.name;
        if (m_summary) {
            m_summary->updateActiveFacet(task.name, task.color);
        }
        else {
            m_trayIcon->showMessage("", task.name, QSystemTrayIcon::Information);
        }
    });

    // Setup BT client
    connect(m_btClient, &TimeFlipBTClient::connected, this, &TrayApplication::handleConnectionToDevice);
    connect(m_btClient, &TimeFlipBTClient::disconnected, this, &TrayApplication::handleDisconnectionFromDevice);
    connect(m_btClient, &TimeFlipBTClient::scanFinished, this, &TrayApplication::handleScanFinished);
    connect(m_btClient, &TimeFlipBTClient::error, this, &TrayApplication::handleError);

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    Config &config = Config::instance();
    config.load();

    // If not configured, show configuration dialog
    if (!config.isValid()) {
        showConfiguration();
    }
    else {
        applyConfiguration();
    }
}

void TrayApplication::handleConnectionToDevice()
{
    m_trayIcon->showMessage("", "Connected to a TimeFlip device", QSystemTrayIcon::Information);
    qDebug() << "Requesting active facet info...";
    m_btClient->getActiveFacet();
}

void TrayApplication::handleDisconnectionFromDevice()
{
    m_trayIcon->showMessage("", "Disconnected from a TimeFlip device", QSystemTrayIcon::Information);
}

void TrayApplication::handleScanFinished()
{
    m_trayIcon->showMessage("", "No TimeFlip device detected...", QSystemTrayIcon::Information);
}

void TrayApplication::handleError(const QString &errorString)
{
    m_trayIcon->showMessage("", errorString, QSystemTrayIcon::Critical);
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


void TrayApplication::showConfiguration()
{
    if (m_configuration == nullptr) {
        m_configuration = std::make_unique<Configuration>(nullptr);
        m_configuration->setWindowFlags(m_configuration->windowFlags() | Qt::WindowStaysOnTopHint);
        connect(m_configuration.get(), &Configuration::configurationUpdated, this, &TrayApplication::applyConfiguration);
    }

    if (m_configuration->isHidden()) {
        m_configuration->show();
    } else {
        m_configuration->hide();
    }
}

void TrayApplication::applyConfiguration()
{
    const Credentials credentials {
        .email = Config::instance().email,
        .password = Config::instance().password
    };
    m_apiClient->setCredentials(credentials);
    if (credentials.isValid()) {
        m_apiClient->authenticate();
    }
}

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

#pragma once

#include <QObject>
#include <QSystemTrayIcon>

class Configuration;
class Summary;

namespace TimeFlipApi {
    class TimeFlipApiClient;
}

namespace TimeFlipBT {
    class TimeFlipBTClient;
}

class TrayApplication : public QObject
{
    Q_OBJECT
public:
    TrayApplication(QObject *parent = nullptr);
    ~TrayApplication();

private slots:
    void handleConnectionToDevice();
    void handleDisconnectionFromDevice();
    void handleScanFinished();
    void handleError(const QString &errorString);

    void handleTrayIconActivation(QSystemTrayIcon::ActivationReason reason);

    void showConfiguration();
    void applyConfiguration();

    void showSummary();

    void positionWidgetBottomRight(QWidget *widget);

private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    std::unique_ptr<Configuration> m_configuration;
    std::unique_ptr<Summary> m_summary;
    TimeFlipApi::TimeFlipApiClient *m_apiClient;
    TimeFlipBT::TimeFlipBTClient *m_btClient;
};

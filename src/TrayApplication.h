#pragma once

#include <QObject>
#include <QSystemTrayIcon>

class Configuration;
class Summary;

namespace TimeFlipApi {
    class TimeFlipApiClient;
}

class TrayApplication : public QObject
{
    Q_OBJECT
public:
    TrayApplication();
    ~TrayApplication();

private slots:
    void handleTrayIconActivation(QSystemTrayIcon::ActivationReason reason);

    void showSummary();
    void showConfiguration();

    void applyConfiguration();

    void positionWidgetBottomRight(QWidget *widget);

private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    std::unique_ptr<Configuration> m_configuration;
    std::unique_ptr<Summary> m_summary;
    TimeFlipApi::TimeFlipApiClient *m_apiClient;
};

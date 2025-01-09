#pragma once

#include <QObject>
#include <QSystemTrayIcon>

class Configuration;
class Summary;

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

    void handleConfigurationUpdate();

    void positionWidgetBottomRight(QWidget *widget);

private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    std::unique_ptr<Configuration> m_configuration;
    std::unique_ptr<Summary> m_summary;
};

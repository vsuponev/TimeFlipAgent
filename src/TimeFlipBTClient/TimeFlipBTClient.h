#pragma once

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QObject>

class QLowEnergyController;
class QBluetoothDeviceDiscoveryAgent;

namespace TimeFlipBT {

class TimeFlipBTClient : public QObject
{
    Q_OBJECT

public:
    TimeFlipBTClient(QObject *parent = nullptr);

    void startDiscovery();

private slots:
    void connectToDevice(const QBluetoothDeviceInfo &device);

    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void handleScanFinished();
    void connectedToDevice();
    void serviceDiscovered(const QBluetoothUuid &newService);

    void handleDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error err);
    void handleConnectionError(QLowEnergyController::Error err);

signals:
    void connected();
    void disconnected();
    void scanFinished();
    void error(const QString &errorString);

private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;
    QLowEnergyController *m_controller = nullptr;
    QLowEnergyService *m_service = nullptr;
};

} // namespace TimeFlipBT

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
    ~TimeFlipBTClient();

    void startDiscovery();

    void getActiveFacet();

private slots:
    void connectToDevice(const QBluetoothDeviceInfo &device);

    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void handleScanFinished();
    void connectedToDevice();
    void serviceDiscovered(const QBluetoothUuid &newService);
    void enableCharacteristicNotifications(const QLowEnergyCharacteristic &info);

    void handleDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error err);
    void handleConnectionError(QLowEnergyController::Error err);
    void handleServiceError(QLowEnergyService::ServiceError err);
    void handleServiceStateChanged(QLowEnergyService::ServiceState state);
    void handleCharacteristicData(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void handleFacetCharacteristic(const QLowEnergyCharacteristic &info, const QByteArray &value);

signals:
    void connected();
    void disconnected();
    void scanFinished();
    void error(const QString &errorString);
    void facetChanged(int facet);

private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;
    QLowEnergyController *m_controller = nullptr;
    QLowEnergyService *m_service = nullptr;
};

} // namespace TimeFlipBT

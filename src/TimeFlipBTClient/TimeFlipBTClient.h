#pragma once

#include <QBluetoothDeviceInfo>
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
    void scanFinished();

    void handleError(QLowEnergyController::Error newError);

signals:
    void connected();
    void disconnected();

private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = nullptr;
    QLowEnergyController *controller = nullptr;
};

} // namespace TimeFlipBT

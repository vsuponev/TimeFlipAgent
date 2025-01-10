#include "TimeFlipBTClient.h"

#include <QDebug>
#include <QtBluetooth>

namespace {

const QString TIMEFLIP_DEVICE_NAME = QStringLiteral("TimeFlip");

} // anonymous namespace

namespace TimeFlipBT {

TimeFlipBTClient::TimeFlipBTClient(QObject *parent)
    : QObject(parent)
    , discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this))
{
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &TimeFlipBTClient::deviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &TimeFlipBTClient::scanFinished);
}

void TimeFlipBTClient::startDiscovery()
{
    qDebug() << "Discovering devices...";
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void TimeFlipBTClient::connectToDevice(const QBluetoothDeviceInfo &device) {
    qDebug() << "Connecting to" << device.name();
    controller = QLowEnergyController::createCentral(device, this);

    connect(controller, &QLowEnergyController::connected, this, &TimeFlipBTClient::connected);
    connect(controller, &QLowEnergyController::disconnected, this, &TimeFlipBTClient::disconnected);
    connect(controller, &QLowEnergyController::errorOccurred, this, &TimeFlipBTClient::handleError);
    controller->connectToDevice();
}

void TimeFlipBTClient::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found device:" << device.name() << device.address().toString();
    if (device.name().contains(TIMEFLIP_DEVICE_NAME, Qt::CaseInsensitive)) {
        discoveryAgent->stop();
        connectToDevice(device);
    }
}

void TimeFlipBTClient::scanFinished()
{

}

void TimeFlipBTClient::handleError(QLowEnergyController::Error newError)
{

}

} // namespace TimeFlipBT

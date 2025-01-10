#include "TimeFlipBTClient.h"

#include <QDebug>
#include <QtBluetooth>

namespace {

const QString TIMEFLIP_DEVICE_NAME = QStringLiteral("TimeFlip");

const QBluetoothUuid TIMEFLIP_SERVICE_UUID("F1196F50-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_EVENTS_DATA_UUID("F1196F51-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_FACETS_UUID("F1196F52-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_COMMAND_RESULT_OUTPUT_UUID("F1196F53-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_COMMAND_UUID("F1196F54-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_DOUBLE_TAP_DEFINITION_UUID("F1196F55-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_SYSTEM_STATE_UUID("F1196F56-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_PASSWORD_CHECK_UUID("F1196F57-71A4-11E6-BDF4-0800200C9A66");
const QBluetoothUuid TIMEFLIP_HISTORY_DATA_UUID("F1196F58-71A4-11E6-BDF4-0800200C9A66");

} // anonymous namespace

namespace TimeFlipBT {

TimeFlipBTClient::TimeFlipBTClient(QObject *parent)
    : QObject(parent)
    , m_discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this))
{
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &TimeFlipBTClient::deviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &TimeFlipBTClient::handleScanFinished);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &TimeFlipBTClient::handleDiscoveryError);
}

void TimeFlipBTClient::startDiscovery()
{
    qDebug() << "Discovering devices...";
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void TimeFlipBTClient::connectToDevice(const QBluetoothDeviceInfo &device) {
    qDebug() << "Connecting to" << device.name();
    m_controller = QLowEnergyController::createCentral(device, this);

    connect(m_controller, &QLowEnergyController::connected, this, &TimeFlipBTClient::connectedToDevice);
    connect(m_controller, &QLowEnergyController::disconnected, this, &TimeFlipBTClient::disconnected);
    connect(m_controller, &QLowEnergyController::errorOccurred, this, &TimeFlipBTClient::handleConnectionError);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &TimeFlipBTClient::serviceDiscovered);
    m_controller->connectToDevice();
}

void TimeFlipBTClient::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found device:" << device.name() << device.address().toString();
    if (device.name().contains(TIMEFLIP_DEVICE_NAME, Qt::CaseInsensitive)) {
        m_discoveryAgent->stop();
        connectToDevice(device);
    }
}

void TimeFlipBTClient::handleScanFinished()
{
    emit scanFinished();
}

void TimeFlipBTClient::connectedToDevice()
{
    qDebug() << "Connected to a TimeFlip device";
    m_controller->discoverServices();
}

void TimeFlipBTClient::serviceDiscovered(const QBluetoothUuid &newService)
{
    qDebug() << "Discovered service:" << newService.toString();
    if (newService == TIMEFLIP_SERVICE_UUID) {
        if (m_service) {
            m_service->deleteLater();
        }
        m_service = m_controller->createServiceObject(newService, this);
        if (m_service == nullptr) {
            qDebug() << "Failed to create service object";
            return;
        }
        qDebug() << "Service created";
        emit connected();
    }
}

void TimeFlipBTClient::handleDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error err)
{
    emit error(m_discoveryAgent->errorString());
}

void TimeFlipBTClient::handleConnectionError(QLowEnergyController::Error err)
{
    emit error(m_controller->errorString());
}

} // namespace TimeFlipBT

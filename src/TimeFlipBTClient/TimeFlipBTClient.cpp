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

const QBluetoothUuid CLIENT_CHARACTERISTIC_CONFIGURATION_DESCRIPTOR("00002902-0000-1000-8000-00805f9b34fb");


QString serviceErrorToString(QLowEnergyService::ServiceError error) {
    switch (error) {
    case QLowEnergyService::NoError:
        return "No error";
    case QLowEnergyService::OperationError:
        return "Operation error";
    case QLowEnergyService::CharacteristicWriteError:
        return "Characteristic write error";
    case QLowEnergyService::DescriptorWriteError:
        return "Descriptor write error";
    case QLowEnergyService::UnknownError:
        return "Unknown error";
    case QLowEnergyService::CharacteristicReadError:
        return "Characteristic read error";
    case QLowEnergyService::DescriptorReadError:
        return "Descriptor read error";
    default:
        return "Unknown error code";
    }
}

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

TimeFlipBTClient::~TimeFlipBTClient()
{
    if (m_controller) {
        if (m_controller->state() == QLowEnergyController::ConnectedState) {
            m_controller->disconnectFromDevice();
            qDebug() << "Disconnected from the TimeFlip device.";
        }
    }
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
        qDebug() << "Service created, discovering details...";

        connect(m_service, &QLowEnergyService::characteristicChanged, this, &TimeFlipBTClient::handleCharacteristicData);
        connect(m_service, &QLowEnergyService::characteristicRead, this, &TimeFlipBTClient::handleCharacteristicData);
        connect(m_service, &QLowEnergyService::stateChanged, this, &TimeFlipBTClient::handleServiceStateChanged);
        connect(m_service, &QLowEnergyService::errorOccurred, this, &TimeFlipBTClient::handleServiceError);

        m_service->discoverDetails();
    }
}

void TimeFlipBTClient::enableCharacteristicNotifications(const QLowEnergyCharacteristic &info)
{
    qDebug() << "Characteristic descriptors:";
    for (const auto &descriptor : info.descriptors()) {
        qDebug() << descriptor.uuid().toString() << descriptor.name();
    }
    if (info.properties() & QLowEnergyCharacteristic::Notify) {
        QLowEnergyDescriptor cccdDescriptor = info.descriptor(CLIENT_CHARACTERISTIC_CONFIGURATION_DESCRIPTOR);
        if (cccdDescriptor.isValid()) {
            m_service->writeDescriptor(cccdDescriptor, QByteArray::fromHex("0100")); // Enable notifications
            qDebug() << "Notifications enabled for facets characteristic.";
        } else {
            qWarning() << "CCCD descriptor not found for the characteristic" << info.uuid().toString();
        }
    } else {
        qWarning() << "Characteristic does not support notifications!";
    }}

void TimeFlipBTClient::handleCharacteristicData(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    if (!info.isValid()) {
        emit error("Invalid characteristic");
    }
    if (info.uuid() == TIMEFLIP_FACETS_UUID) {
        handleFacetCharacteristic(info, value);
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

void TimeFlipBTClient::handleServiceError(QLowEnergyService::ServiceError err)
{
    qDebug().noquote() << "Service error:" << err << ": " << serviceErrorToString(err);
    emit error(serviceErrorToString(err));
}

void TimeFlipBTClient::handleServiceStateChanged(QLowEnergyService::ServiceState state)
{
    if (state != QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "Skipping service state processing: " << state;
        return;
    }
    const auto characteristics = m_service->characteristics();
    qDebug() << "Service discovery finished. Characteristics:";
    for (const auto &charInfo : characteristics) {
        qDebug().noquote() << charInfo.uuid().toString() << ": " << charInfo.name();
        if (charInfo.uuid() == TIMEFLIP_FACETS_UUID) {
            enableCharacteristicNotifications(charInfo);
        }
    }
    emit connected();
}

void TimeFlipBTClient::getActiveFacet()
{
    QLowEnergyCharacteristic facetsChar = m_service->characteristic(TIMEFLIP_FACETS_UUID);
    m_service->readCharacteristic(facetsChar);
}

void TimeFlipBTClient::handleFacetCharacteristic(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    if (info.uuid() != TIMEFLIP_FACETS_UUID) {
        qCritical("Invalid characteristic");
        return;
    }
    if (value.isEmpty()) {
        qCritical("Invalid characteristic value");
        return;
    }
    const quint8 activeFacet = static_cast<quint8>(value[0]);
    emit facetChanged(activeFacet);
}

} // namespace TimeFlipBT

#include "ble_server.h"
#include "ble_types.h"
#include "ble_config.h"
#include <Arduino.h>
#include <NimBLEDevice.h>


namespace orimer::ble {

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
public:
    explicit CharacteristicCallbacks(BleServer* owner)
        : m_pOwner(owner) {}

    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override
    {
        auto value = pCharacteristic->getValue();
        if (value.size() != sizeof(BlePacket))
            return;

        BlePacket packet{};
        memcpy(&packet, value.data(), sizeof(BlePacket));

        m_pOwner->OnReceive(packet);
    }

private:
    BleServer* m_pOwner;
};

class ServerCallbacks : public NimBLEServerCallbacks
{
public:
    explicit ServerCallbacks(BleServer* owner)
        : m_pOwner(owner)
    {}

    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override
    {
        Serial.println("[BLE][Server] Client connected");
        const auto connHandle = connInfo.getConnHandle();
        ConnectionInfo info{};
        info.handle = connHandle;
        info.rssi = 0;

        m_pOwner->OnConnected(info);

        pServer->startAdvertising();
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
    {
        Serial.printf("[BLE][Server] Client disconnected, reason=%d\n", reason);
        const auto connHandle = connInfo.getConnHandle();
        ConnectionInfo info{};
        info.handle = connHandle;
        info.rssi = 0;

        m_pOwner->OnDisconnected(info);
    }

private:
    BleServer* m_pOwner;
};

BleServer::BleServer()
    : m_IsConnected(false)
{
    m_Connections.clear();
}

void BleServer::Begin()
{
    Serial.println("[BLE][Server] Begin");

    NimBLEDevice::init(orimer::ble::atom::ServerName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    // NimBLEDevice::setMaxConnections(ClientCountMax);

    NimBLEServer* pServer =
        NimBLEDevice::createServer();
    pServer->setCallbacks(
        new ServerCallbacks(this)
    );

    NimBLEService* pService =
        pServer->createService(
            orimer::ble::atom::ServiceUuid
        );

    m_pChar =
        pService->createCharacteristic(
            orimer::ble::atom::CharacteristicUuid,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY
        );
    
    m_pChar->setCallbacks(
        new CharacteristicCallbacks(this)
    );

    pService->start();

    m_pAdvertising = NimBLEDevice::getAdvertising();
    
    NimBLEAdvertisementData advData;
    NimBLEAdvertisementData scanData;

    advData.setName(orimer::ble::atom::ServerName);
    advData.addServiceUUID(orimer::ble::atom::ServiceUuid);
    scanData.addServiceUUID(orimer::ble::atom::ServiceUuid);

    m_pAdvertising->setAdvertisementData(advData);
    m_pAdvertising->setScanResponseData(scanData);

    m_pAdvertising->start();

    Serial.println("[BLE][Server] Advertising start");
}

void BleServer::Update()
{
    if (!m_IsConnected || m_pChar == nullptr)
    {
        Serial.println("null data");
        return;
    }
    m_pChar->notify();
    Serial.println("notify done");
}

void BleServer::SendPacket(const BlePacket& packet)
{
    m_pChar->setValue(
        reinterpret_cast<const uint8_t*>(&packet),
        sizeof(BlePacket)
    );
}

BlePacket BleServer::GetPacket() const
{
    return m_Packet;
}

bool BleServer::IsConnected() const
{
    return m_IsConnected;
}

void BleServer::OnConnected(const ConnectionInfo& info)
{
    m_Connections[info.handle] = info;
    m_IsConnected = true;
}

void BleServer::OnDisconnected(const ConnectionInfo& info)
{
    m_IsConnected = false;
    if( m_Connections.find(info.handle) != m_Connections.end() )
    {
        m_Connections.erase(info.handle);
    }
    else
    {
        Serial.println("[BLE][Server] Warning: ConnHandle mismatch on disconnect");
    }

    if (m_pAdvertising != nullptr)
    {
        Serial.println("[BLE][Server] Restart advertising");
        m_pAdvertising->start();
    }
}

void BleServer::OnReceive(BlePacket packet)
{
    m_Packet = packet;
}

} // namespace orimer::ble

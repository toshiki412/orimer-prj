#include "ble_server.h"

#include <Arduino.h>
#include <NimBLEDevice.h>

namespace {

constexpr char k_ServiceUuid[] =
    "4fafc201-1fb5-459e-8fcc-c5c9c331914b";

constexpr char k_CharacteristicUuid[] =
    "beb5483e-36e1-4688-b7f5-ea07361b26a8";

NimBLECharacteristic* g_pCharacteristic = nullptr;

} // namespace

namespace orimer::ble {

class ServerCallbacks : public NimBLEServerCallbacks
{
public:
    explicit ServerCallbacks(BleServer* owner)
        : m_pOwner(owner)
    {}

    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override
    {
        Serial.println("[BLE][Server] Client connected");
        m_pOwner->OnConnected();
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
    {
        Serial.printf("[BLE][Server] Client disconnected, reason=%d\n", reason);
        m_pOwner->OnDisconnected();
    }

private:
    BleServer* m_pOwner;
};

BleServer::BleServer()
    : m_IsConnected(false)
{
}

void BleServer::Begin(const char* pDeviceName)
{
    Serial.println("[BLE][Server] Begin");

    NimBLEDevice::init(pDeviceName);

    NimBLEServer* pServer =
        NimBLEDevice::createServer();
    pServer->setCallbacks(
        new ServerCallbacks(this)
    );

    NimBLEService* pService =
        pServer->createService(k_ServiceUuid);

    g_pCharacteristic =
        pService->createCharacteristic(
            k_CharacteristicUuid,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY
        );

    pService->start();

    NimBLEAdvertising* pAdvertising =
        NimBLEDevice::getAdvertising();
    
    NimBLEAdvertisementData advData;
    NimBLEAdvertisementData scanData;

    advData.setName("Atom-Server");
    advData.addServiceUUID(k_ServiceUuid);
    scanData.addServiceUUID(k_ServiceUuid);

    pAdvertising->setAdvertisementData(advData);
    pAdvertising->setScanResponseData(scanData);

    pAdvertising->start();

    Serial.println("[BLE][Server] Advertising start");
}

void BleServer::Update(const ControlState& state)
{
    if (!m_IsConnected || g_pCharacteristic == nullptr)
    {
        Serial.println("null data");
        return;
    }

    g_pCharacteristic->setValue(
        reinterpret_cast<const uint8_t*>(&state),
        sizeof(ControlState)
    );
    g_pCharacteristic->notify();

    Serial.printf(
        "[BLE][Server] Notify btn=0x%04X dir=%d\n",
        state.btn,
        state.dir
    );
}

bool BleServer::IsConnected() const
{
    return m_IsConnected;
}

void BleServer::OnConnected()
{
    m_IsConnected = true;
}

void BleServer::OnDisconnected()
{
    m_IsConnected = false;
}

} // namespace orimer::ble

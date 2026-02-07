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
    explicit ServerCallbacks(BleServer* pOwner)
        : m_pOwner(pOwner)
    {
    }

    void onConnect(NimBLEServer*)
    {
        Serial.println("[BLE][Server] Connected");
        m_pOwner->OnConnected();
    }

    void onDisconnect(NimBLEServer*)
    {
        Serial.println("[BLE][Server] Disconnected");
        m_pOwner->OnDisconnected();
        NimBLEDevice::startAdvertising();
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
            NIMBLE_PROPERTY::NOTIFY
        );

    pService->start();

    NimBLEAdvertising* pAdvertising =
        NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(k_ServiceUuid);
    pAdvertising->start();

    Serial.println("[BLE][Server] Advertising start");
}

void BleServer::Update(const ControlState& state)
{
    if (!m_IsConnected || g_pCharacteristic == nullptr)
    {
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

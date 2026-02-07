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

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
public:
    explicit CharacteristicCallbacks(BleServer* owner)
        : m_pOwner(owner) {}

    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override
    {
        auto value = pCharacteristic->getValue();
        if (value.size() != sizeof(ControlState))
            return;

        ControlState state;
        memcpy(&state, value.data(), sizeof(ControlState));

        m_pOwner->OnReceive(state);
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
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY
        );
    
    g_pCharacteristic->setCallbacks(
        new CharacteristicCallbacks(this)
    );

    pService->start();

    m_pAdvertising = NimBLEDevice::getAdvertising();
    
    NimBLEAdvertisementData advData;
    NimBLEAdvertisementData scanData;

    advData.setName("Atom-Server");
    advData.addServiceUUID(k_ServiceUuid);
    scanData.addServiceUUID(k_ServiceUuid);

    m_pAdvertising->setAdvertisementData(advData);
    m_pAdvertising->setScanResponseData(scanData);

    m_pAdvertising->start();

    Serial.println("[BLE][Server] Advertising start");
}

void BleServer::Update()
{
    if (!m_IsConnected || g_pCharacteristic == nullptr)
    {
        Serial.println("null data");
        return;
    }
    g_pCharacteristic->notify();
    Serial.println("notify done");
}

void BleServer::SendState(const ControlState& state)
{
    g_pCharacteristic->setValue(
        reinterpret_cast<const uint8_t*>(&state),
        sizeof(ControlState)
    );

    Serial.printf(
        "[BLE][Server] SetValue btn=0x%04X dir=%d\n",
        state.btn,
        state.dir
    );
}

ControlState BleServer::GetState() const
{
    return m_State;
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
    
    if (m_pAdvertising != nullptr)
    {
        Serial.println("[BLE][Server] Restart advertising");
        m_pAdvertising->start();
    }
}

void BleServer::OnReceive(ControlState state)
{
    m_State = state;
}

} // namespace orimer::ble

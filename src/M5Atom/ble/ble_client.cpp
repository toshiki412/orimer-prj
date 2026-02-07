#include "ble_client.h"

#include <Arduino.h>
#include <NimBLEDevice.h>

namespace
{
constexpr char k_ServiceUuid[] =
    "4fafc201-1fb5-459e-8fcc-c5c9c331914b";

constexpr char k_CharacteristicUuid[] =
    "beb5483e-36e1-4688-b7f5-ea07361b26a8";
}

namespace orimer::ble {

static BleClient* g_pSelf = nullptr;

class ClientCallbacks : public NimBLEClientCallbacks
{
public:
    explicit ClientCallbacks(BleClient* pOwner)
        : m_pOwner(pOwner)
    {
    }

    void onConnect(NimBLEClient*)
    {
        Serial.println("[BLE][Client] Connected");
        m_pOwner->OnConnected();
    }

    void onDisconnect(NimBLEClient*)
    {
        Serial.println("[BLE][Client] Disconnected");
        m_pOwner->OnDisconnected();
    }

private:
    BleClient* m_pOwner;
};

void NotifyCallback(
    NimBLERemoteCharacteristic* pChar,
    uint8_t* pData,
    size_t length,
    bool isNotify
)
{
    if (g_pSelf == nullptr)
    {
        return;
    }

    if (length != sizeof(ControlState))
    {
        return;
    }

    memcpy(
        &g_pSelf->m_State,
        pData,
        sizeof(ControlState)
    );
}

BleClient::BleClient()
    : m_IsConnected(false)
{
    m_State.btn = 0;
    m_State.dir = Neutral;
}

void BleClient::Begin()
{
    g_pSelf = this;

    NimBLEDevice::init("");

    NimBLEScan* pScan =
        NimBLEDevice::getScan();
    pScan->setActiveScan(true);

    pScan->start(5, false);
    NimBLEScanResults results =
        pScan->getResults();

    for (int i = 0; i < results.getCount(); ++i)
    {
        const NimBLEAdvertisedDevice* pDevice =
            results.getDevice(i);
        
        NimBLEUUID serviceUuid(k_ServiceUuid);
        if (!pDevice->isAdvertisingService(serviceUuid))
        {
            continue;
        }

        NimBLEClient* pClient =
            NimBLEDevice::createClient();
        pClient->setClientCallbacks(
            new ClientCallbacks(this)
        );

        if (!pClient->connect(pDevice))
        {
            continue;
        }

        NimBLERemoteService* pService =
            pClient->getService(k_ServiceUuid);
        if (pService == nullptr)
        {
            continue;
        }

        NimBLERemoteCharacteristic* pChar =
            pService->getCharacteristic(
                k_CharacteristicUuid
            );
        if (pChar == nullptr)
        {
            continue;
        }

        if (pChar->canNotify())
        {
            pChar->subscribe(true, NotifyCallback);
            Serial.println(
                "[BLE][Client] Subscribed"
            );
        }

        break;
    }
}

void BleClient::Update()
{
}

bool BleClient::IsConnected() const
{
    return m_IsConnected;
}

const ControlState& BleClient::GetState() const
{
    return m_State;
}

void BleClient::OnConnected()
{
    m_IsConnected = true;
}

void BleClient::OnDisconnected()
{
    m_IsConnected = false;
}

} // namespace orimer::ble

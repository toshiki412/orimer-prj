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

class ClientCallbacks : public NimBLEClientCallbacks
{
public:
    explicit ClientCallbacks(BleClient* pOwner)
        : m_pOwner(pOwner)
    {
    }

    void onConnect(NimBLEClient* pClient) override
    {
        Serial.println("[BLE][Client] Connected");
        m_pOwner->OnConnected();
    }

    void onDisconnect(NimBLEClient* pClient, int reason) override
    {
        Serial.println("[BLE][Client] Disconnected");
        m_pOwner->OnDisconnected();
    }

private:
    BleClient* m_pOwner;
};

class ScanCallbacks : public NimBLEScanCallbacks {
public:
    explicit ScanCallbacks(BleClient* owner) : m_pOwner(owner) {}

    void onResult(const NimBLEAdvertisedDevice* device) override {

        if (!device->haveName()) return;
        if (device->getName() != "Atom-Server") return;

        Serial.printf(
            "[SCAN] name='%s' addr=%s rssi=%d hasName=%d\n",
            device->getName().c_str(),
            device->getAddress().toString().c_str(),
            device->getRSSI(),
            device->haveName()
        );

        Serial.println("[BLE][Client] Target found, stop scan");
        NimBLEDevice::getScan()->stop();   // ★ 必須
        m_pOwner->Register(device->getAddress());
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
    if (length != sizeof(ControlState))
    {
        return;
    }

    ControlState state{};
    memcpy(
        &state,
        pData,
        sizeof(ControlState)
    );

    BleClient::GetInstance()->SetState(state);
}

BleClient::BleClient()
    : m_IsConnected(false)
{
    m_State.btn = 0;
    m_State.dir = Neutral;
}

void BleClient::Begin()
{
    NimBLEDevice::init("Atom-Client");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    Serial.println("[BLE][Client] Begin");

    this->StartScan();
}

void BleClient::Register(const NimBLEAddress& addr)
{
    m_DeviceAddr = addr;
    m_HasDeviceAddr = true;
}

void BleClient::UnRegister()
{
}

void BleClient::StartScan()
{
    auto pScan = NimBLEDevice::getScan();
    pScan->clearResults();
    pScan->setActiveScan(true);
    pScan->setInterval(45);
    pScan->setWindow(15);
    pScan->setDuplicateFilter(false);
    pScan->setScanCallbacks(new ScanCallbacks(this), true);
    pScan->start(5, true);
    Serial.println("[BLE][Client] start scan");
}

bool BleClient::TryConnect()
{
    if(!m_HasDeviceAddr)
    {
        Serial.println("[BLE][Client] No server addr yet");
        return false;
    }

    Serial.println("[BLE][Client] Try connect...");

    m_pClient = NimBLEDevice::createClient();
    m_pClient->setClientCallbacks(
        new ClientCallbacks(this)
    );

    if (!m_pClient->connect(m_DeviceAddr))
    {
        return false;
    }

    auto pService = m_pClient->getService(k_ServiceUuid);
    if (pService == nullptr)
    {
        return false;
    }

    m_pChar = pService->getCharacteristic(k_CharacteristicUuid);
    if (m_pChar == nullptr)
    {
        return false;
    }

    if (m_pChar->canNotify())
    {
        m_pChar->subscribe(true, NotifyCallback);
        Serial.println(
            "[BLE][Client] Subscribed"
        );
    }

    return true;
}

void BleClient::Update()
{
    if (this->IsConnected())
    {
        return;
    }

    if(!m_HasDeviceAddr)
    {
        StartScan();
        return;
    }

    const uint32_t now = millis();
    if (now - m_LastRetryMs < 3000)
    {
        return;
    }

    m_LastRetryMs = now;

    this->TryConnect();
}

bool BleClient::IsConnected() const
{
    return m_IsConnected;
}

void BleClient::SetState(ControlState state)
{
    m_State = state;
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
    m_pChar = nullptr;
    m_HasDeviceAddr = false;

    if (m_pClient)
    {
        NimBLEDevice::deleteClient(m_pClient);
        m_pClient = nullptr;
    }
}

} // namespace orimer::ble

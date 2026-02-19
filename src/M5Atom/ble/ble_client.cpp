#include "ble_client.h"
#include "ble_config.h"
#include "../config.h"

#if defined(USE_COOSPO)
    #include "../coospo/coospo_api.h"
#endif

#include <Arduino.h>
#include <NimBLEDevice.h>

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

    void onResult(const NimBLEAdvertisedDevice* device) override 
    {
        Serial.printf(
            "[SCAN] name='%s' addr=%s rssi=%d hasName=%d\n",
            device->getName().c_str(),
            device->getAddress().toString().c_str(),
            device->getRSSI(),
            device->haveName()
        );

        if(!device->isAdvertisingService(NimBLEUUID(m_pOwner->GetTargetServiceUUID()))) return;

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
#if defined(USE_COOSPO)
    orimer::coospo::Update(pData, length);
    return;
#endif

    if (length != sizeof(BlePacket))
    {
        return;
    }


    BlePacket packet{};
    memcpy(
        &packet,
        pData,
        sizeof(BlePacket)
    );

    BleClient::GetInstance()->SetPacket(packet);
}

BleClient::BleClient()
    : m_IsConnected(false)
{
}

void BleClient::SetTargetDeviceName(const std::string& name)
{
    m_TargetDeviceName = name;
}

void BleClient::SetTargetServiceUUID(const std::string& uuid)
{
    m_TargetServiceUUID = uuid;
}

void BleClient::SetTargetCharacteristicUUID(const std::string& uuid)
{
    m_TargetCharacteristicUUID = uuid;
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
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setDuplicateFilter(false);
    pScan->setScanCallbacks(new ScanCallbacks(this), true);
    pScan->start(5, true);
    // Serial.println("[BLE][Client] start scan");
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

    auto pService = m_pClient->getService(NimBLEUUID(m_TargetServiceUUID));
    if (pService == nullptr)
    {
        return false;
    }

    m_pChar = pService->getCharacteristic(NimBLEUUID(m_TargetCharacteristicUUID));
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

int BleClient::GetRssi() const
{
    if (m_pClient == nullptr)
    {
        return 0;
    }

    return m_Rssi;
}

void BleClient::Update()
{
    if (this->IsConnected())
    {
        if (m_pClient != nullptr)
        {
            constexpr float k_RssiFilterAlpha = 0.1f;
            const float filteredRssi = (1.0f - k_RssiFilterAlpha) * m_Rssi + k_RssiFilterAlpha * m_pClient->getRssi();
            m_Rssi = static_cast<int>(filteredRssi);
        }

        Serial.printf("[BLE][Client] Connected RSSI=%d\n", this->GetRssi());
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

void BleClient::SetPacket(BlePacket packet)
{
    m_Packet = packet;
}

const BlePacket& BleClient::GetPacket() const
{
    return m_Packet;
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

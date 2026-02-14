#pragma once

#include <cstdint>
#include <cstddef>   // size_t
#include <NimBLEDevice.h>

// forward declaration（ヘッダでは宣言だけ）
class NimBLERemoteCharacteristic;

#include "ble_types.h"
#include "ble_config.h"

namespace orimer::ble
{

class BleClient;

// Notify callback（friend 用）
void NotifyCallback(
    NimBLERemoteCharacteristic* pChar,
    uint8_t* pData,
    size_t length,
    bool isNotify
);

class BleClient
{
    friend void NotifyCallback(
        NimBLERemoteCharacteristic*,
        uint8_t*,
        size_t,
        bool
    );

public:
    BleClient();

    static BleClient* GetInstance()
    {
        static BleClient s_Instance;
        return &s_Instance;
    }

    void SetTargetDeviceName(const std::string& name);
    void SetTargetServiceUUID(const std::string& uuid);
    void SetTargetCharacteristicUUID(const std::string& uuid);

    std::string GetTargetDeviceName() const { return m_TargetDeviceName; }
    std::string GetTargetServiceUUID() const { return m_TargetServiceUUID; }
    std::string GetTargetCharacteristicUUID() const { return m_TargetCharacteristicUUID; }

    void Begin();
    void Update();
    void Register(const NimBLEAddress& addr);
    void UnRegister();

    bool IsConnected() const;
    void SetPacket(BlePacket packet);
    const BlePacket& GetPacket() const;

    int GetRssi() const;

    void OnConnected();
    void OnDisconnected();

private:
    void StartScan();
    bool TryConnect();

    NimBLEClient* m_pClient = nullptr;
    NimBLERemoteCharacteristic* m_pChar = nullptr;
    NimBLEAddress m_DeviceAddr{};
    bool m_HasDeviceAddr = false;
    uint32_t m_LastRetryMs{};
    bool m_IsConnected { false };
    BlePacket m_Packet{};
    int m_Rssi { 0 };

    std::string m_TargetDeviceName{orimer::ble::atom::ServerName};
    std::string m_TargetServiceUUID{orimer::ble::atom::ServiceUuid};
    std::string m_TargetCharacteristicUUID{orimer::ble::atom::CharacteristicUuid};
};

} // namespace orimer::ble

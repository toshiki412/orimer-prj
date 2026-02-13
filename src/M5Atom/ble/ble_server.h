#pragma once

#include <map> 
#include <cstdint>
#include <NimBLEDevice.h>
#include "ble_types.h"

namespace orimer::ble {

class BleServer
{
    friend class ServerCallbacks;
public:
    BleServer();

    static BleServer* GetInstance()
    {
        static BleServer s_Instance;
        return &s_Instance;
    }

    void Begin();
    void Update();
    void SendPacket(const BlePacket& packet);
    BlePacket GetPacket() const;
    bool IsConnected() const;

    void OnConnected(const ConnectionInfo& info);
    void OnDisconnected(const ConnectionInfo& info);
    void OnReceive(BlePacket state);

private:
    static constexpr int ClientCountMax = 3;
    NimBLECharacteristic* m_pChar = nullptr;
    NimBLEAdvertising* m_pAdvertising = nullptr;
    BlePacket m_Packet{};
    std::map<uint16_t, ConnectionInfo> m_Connections;
    bool m_IsConnected;
};

} // namespace orimer::ble

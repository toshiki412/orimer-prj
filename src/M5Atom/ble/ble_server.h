#pragma once

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
    void SendState(const ControlState& state);
    ControlState GetState() const;
    bool IsConnected() const;

    void OnConnected();
    void OnDisconnected();
    void OnReceive(ControlState state);

private:
    NimBLECharacteristic* m_pChar = nullptr;
    NimBLEAdvertising* m_pAdvertising = nullptr;
    ControlState m_State{};
    bool m_IsConnected;
};

} // namespace orimer::ble

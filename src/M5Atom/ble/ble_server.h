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

    void Begin(const char* pDeviceName);
    void Update();
    void SendState(const ControlState& state);
    ControlState GetState() const;
    bool IsConnected() const;

    void OnConnected();
    void OnDisconnected();
    void OnReceive(ControlState state);

private:
    NimBLEAdvertising* m_pAdvertising = nullptr;
    ControlState m_State{};
    bool m_IsConnected;
};

} // namespace orimer::ble

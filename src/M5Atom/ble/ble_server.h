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
    void Update(const ControlState& state);

    bool IsConnected() const;

    void OnConnected();
    void OnDisconnected();
    void OnReceive(ControlState state);

private:
    NimBLEAdvertising* m_pAdvertising = nullptr;
    bool m_IsConnected;
};

} // namespace orimer::ble

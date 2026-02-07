#pragma once

#include <cstdint>
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

private:
    bool m_IsConnected;
};

} // namespace orimer::ble

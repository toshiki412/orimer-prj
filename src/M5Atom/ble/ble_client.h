#pragma once

#include <cstdint>
#include <cstddef>   // size_t

// forward declaration（ヘッダでは宣言だけ）
class NimBLERemoteCharacteristic;

#include "ble_types.h"

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

    void Begin();
    void Update();

    bool IsConnected() const;
    const ControlState& GetState() const;

    void OnConnected();
    void OnDisconnected();

private:
    bool m_IsConnected { false };
    ControlState m_State {};
};

} // namespace orimer::ble

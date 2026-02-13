#pragma once
#include <cstdint>

namespace orimer::ble {

struct ConnectionInfo
{
    uint16_t handle;
    int rssi;
};

// 16bytes
struct BlePacket
{
    uint8_t type;
    uint8_t seq;
    uint8_t data[14];
};

/**
 * @brief BLE 動作モード
 *
 * None   : 初期化前 / 未使用
 * Server : BLE GATT Server として動作
 * Client : BLE GATT Client として動作
 */
enum class Mode : uint8_t
{
    None = 0,
    Server,
    Client,
};

} // namespace orimer::ble


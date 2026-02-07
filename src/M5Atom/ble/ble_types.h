#pragma once
#include <cstdint>

namespace orimer::ble {

enum StickDir : uint8_t
{
    Neutral = 0,
    Up,
    Down,
    Left,
    Right,
};

struct ControlState
{
    uint16_t btn;
    uint8_t  dir;
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


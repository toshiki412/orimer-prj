#pragma once
#include <stdint.h>

namespace orimer::fsr
{

/**
 * @brief FSR 状態データ
 */
struct State
{
    uint16_t raw;        // ADC 生値
    float    normalized; // 0.0 ~ 1.0
    float    force;      // 擬似荷重
    bool     pressed;    // 閾値判定
};

/**
 * @brief FSR API 動作モード
 */
enum class Mode : uint8_t
{
    None = 0,
    Ready,
    Error
};

} // namespace orimer::fsr

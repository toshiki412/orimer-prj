#pragma once
#include <stdint.h>

namespace orimer::fsr
{

/**
 * @brief FSR の状態
 */
struct FsrState
{
    uint16_t raw;      // ADC 生値 (0-4095)
    float    voltage;  // 電圧 [V]
    float    force;    // 推定荷重（正規化 or 仮単位）
};

} // namespace orimer::fsr

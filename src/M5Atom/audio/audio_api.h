#pragma once

#include "audio_types.h"

namespace orimer::audio {

/**
 * @brief Audio 初期化
 */
void Initialize();

/**
 * @brief ビープ音を鳴らす
 */
void BeepEx(uint32_t freq, uint32_t timeMs);

/**
 * @brief ビープ音を鳴らす
 */
void Beep(BeepTone tone);

} // namespace orimer::audio

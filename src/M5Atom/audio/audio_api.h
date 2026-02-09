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
void Beep(BeepTone tone);

} // namespace orimer::audio

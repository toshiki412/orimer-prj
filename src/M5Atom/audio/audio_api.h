#pragma once

#include "audio_types.h"
#include <cstddef>

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

/**
 * @brief マイクから音声データを取得する
 *
 * @param[out] buffer
 *   音声データの格納先バッファ
 *
 * @param[in] samples
 *   取得するサンプル数
 *
 * @return
 *   実際に取得したサンプル数
 */
size_t Read(int16_t* buffer, size_t samples);

/**
 * @brief マイク音量を取得する
 *
 * @return
 *   音量 (0.0 ～ 1.0)
 */
float GetVolume();

/**
 * @brief マイク音声のピッチを取得する
 *
 * @return
 *   ピッチ (Hz)
 */
float GetPitch();

} // namespace orimer::audio

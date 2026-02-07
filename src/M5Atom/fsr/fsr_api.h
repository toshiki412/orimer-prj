#pragma once
#include "fsr_types.h"

namespace orimer::fsr
{

/**
 * @brief FSR を初期化する
 *
 * - ADC を初期化
 * - 内部状態をリセット
 *
 * @param[in] adcPin
 *   FSR を接続した ADC ピン
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 * - loop() 内で呼ばないこと
 */
void Init(uint8_t adcPin);

/**
 * @brief FSR 内部処理を更新する
 *
 * - ADC 読み取り
 * - 正規化
 * - 押下判定
 *
 * @note
 * - loop() 内で毎回呼ぶこと
 */
void Update(void);

/**
 * @brief 最新の FSR 状態を取得する
 *
 * @param[out] state
 *   現在の状態（成功時のみ更新）
 *
 * @return
 * - true  : 正常に取得
 * - false : 未初期化 / エラー
 */
bool GetState(State& state);

/**
 * @brief 押されているかを取得する
 *
 * @return
 * - true  : 押されている
 * - false : 押されていない / 未初期化
 */
bool IsPressed(void);

/**
 * @brief 動作モードを取得する
 */
Mode GetMode(void);

} // namespace orimer::fsr


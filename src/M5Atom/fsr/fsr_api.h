#pragma once
#include "fsr_types.h"

namespace orimer::fsr
{

/**
 * @brief FSR センサを初期化する
 *
 * - 内部状態を初期化
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 * - loop() 内で呼ばないこと
 */
void Initialize();

/**
 * @brief FSR 内部処理を更新する
 *
 * - ADC を読み取る
 * - 内部状態を更新する
 *
 * @note
 * - loop() 内で毎回呼ぶこと
 */
void Update();

/**
 * @brief 最新の FSR 状態を取得する
 *
 * @param[out] state
 *   取得した状態（成功時のみ上書き）
 *
 * @return
 * - true  : 有効なデータを取得
 * - false : 未初期化
 */
bool GetState(FsrState& state);

/**
 * @brief FSR が初期化済みかどうか
 *
 * @return
 * - true  : 初期化済み
 * - false : 未初期化
 */
bool IsInitialized();

} // namespace orimer::fsr

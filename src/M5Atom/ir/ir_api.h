#pragma once
#include "ir_VS1838B_driver.h"
#include "ir_diode_driver.h"
#include <Arduino.h>
#include <stdint.h>

namespace orimer::ir
{

/**
 * @brief IR ダイオード（送信側）を初期化する
 *
 * - 指定ピンで 38kHz PWM を準備する
 *
 * @param[in] pin
 *   IR LED を接続した GPIO 番号
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 */
void InitializeDiode(int pin);

/**
 * @brief IR 受信モジュールを初期化する
 *
 * - VS1838B の入力ピンを設定する
 *
 * @param[in] pin
 *   VS1838B の OUT を接続した GPIO 番号
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 */
void InitializeReceiver(int pin);

/**
 * @brief IR バーストを送信する
 *
 * @param[in] durationMs
 *   38kHz を出力する時間（ms）
 *
 * @return
 * - true  : 送信可能
 * - false : Diode 未初期化
 */
bool Send(uint16_t durationMs);

/**
 * @brief IR 信号を検出しているか取得する
 *
 * @return
 * - true  : 検出中（LOW）
 * - false : 未検出
 *
 * @note
 * - VS1838B はアクティブ LOW
 */
bool IsDetected(void);
} // namespace orimer::ir


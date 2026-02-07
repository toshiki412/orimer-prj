#pragma once
#include "ble_types.h"

namespace orimer::ble
{

/**
 * @brief BLE を Server モードで初期化する
 *
 * - BLE GATT Server を起動し、Advertise を開始する
 * - Characteristic は ControlState を送信する用途
 * - すでに Client として初期化されていた場合は Client を破棄する
 *
 * @param[in] pDeviceName
 *   Advertise に使用するデバイス名（nullptr 不可）
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 * - loop() 内で呼ばないこと
 * - Server / Client の同時使用は不可
 */
void InitServer(const char* pDeviceName);

/**
 * @brief BLE を Client モードで初期化する
 *
 * - 周囲の BLE Server をスキャンし、接続を試みる
 * - 接続対象は ble_types.h の Service UUID を持つもの
 * - すでに Server として初期化されていた場合は Server を破棄する
 *
 * @note
 * - setup() から 1 回だけ呼ぶこと
 * - loop() 内で呼ばないこと
 * - 接続完了までは Receive() は false を返す
 */
void InitClient(void);

/**
 * @brief BLE 内部処理を更新する
 *
 * - スキャン処理
 * - 再接続処理
 * - 内部状態更新
 *
 * @note
 * - loop() 内で毎回呼ぶこと
 */
void Update(void);

/**
 * @brief ControlState を BLE 経由で送信する（Server → Client）
 *
 * @param[in] state
 *   送信する制御データ
 *
 * @return
 * - true  : 正常に送信された
 * - false : 未接続 / Server モードでない / 初期化前
 *
 * @note
 * - Server モード専用
 * - 未接続時に呼んでもクラッシュしない
 * - Notify を使用するため Client 側で onNotify が必要
 */
bool Send(const ControlState& state);

/**
 * @brief BLE 経由で受信した ControlState を取得する
 *
 * @param[out] state
 *   受信した制御データ（成功時のみ上書きされる）
 *
 * @return
 * - true  : 新しいデータを取得
 * - false : 未接続 / Client モードでない / データ未受信
 *
 * @note
 * - 受信データは最後に受信したものが保持される
 * - Server 側からの Notify を前提とする
 */
bool Receive(ControlState& state);

/**
 * @brief BLE 接続状態を取得する
 *
 * @return
 * - true  : 接続中
 * - false : 未接続 / 初期化前
 *
 * @note
 * - Server / Client 両対応
 * - Advertise 中（未接続）は false
 */
bool IsConnected(void);

/**
 * @brief 現在の BLE 動作モードを取得する
 *
 * @return
 * - Mode::None   : 未初期化
 * - Mode::Server : Server 動作中
 * - Mode::Client : Client 動作中
 *
 * @note
 * - デバッグや状態分岐に使用
 */
Mode GetMode(void);

} // namespace orimer::ble


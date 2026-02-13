#pragma once

namespace orimer::action
{

// 初期化
void Initialize();

// 終了処理
void Finalize();

// 毎フレーム更新（loopで呼ぶ）
void Update();

// 手動操作
void ManualMove(float targetV, float targetW);

// 自動走行（速度指定）
void AutoMove(float targetV);

// 目標角速度設定（ライン制御用）
void SetTargetAngular(float targetW);

// 緊急停止
void Stop();

}

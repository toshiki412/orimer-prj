#include "ir_api.h"
#include <Arduino.h>

namespace orimer::ir
{

// =============================
// 内部状態
// =============================

namespace
{
    constexpr int  kPwmChannel = 0;
    constexpr int  kFrequency  = 38000;  // 38kHz
    constexpr int  kResolution = 8;      // 8bit
    constexpr int  kDuty       = 128;    // 50%

    int  g_TxPin        = -1;
    int  g_RxPin        = -1;
    bool g_IsDiodeReady   = false;
    bool g_IsReceiverReady = false;
}

// =============================
// 初期化
// =============================

void InitializeDiode(int pin)
{
    g_TxPin = pin;

    ledcSetup(kPwmChannel, kFrequency, kResolution);
    ledcAttachPin(g_TxPin, kPwmChannel);
    ledcWrite(kPwmChannel, 0);  // 初期は停止

    g_IsDiodeReady = true;
}

void InitializeReceiver(int pin)
{
    g_RxPin = pin;

    pinMode(g_RxPin, INPUT);

    g_IsReceiverReady = true;
}

// =============================
// 送信
// =============================

bool Send(uint16_t durationMs)
{
    if (!g_IsDiodeReady)
        return false;

    ledcWrite(kPwmChannel, kDuty);  // 38kHz ON
    delay(durationMs);
    ledcWrite(kPwmChannel, 0);      // OFF

    return true;
}

// =============================
// 受信検出
// =============================

bool IsDetected(void)
{
    if (!g_IsReceiverReady)
        return false;

    // VS1838B はアクティブ LOW
    return digitalRead(g_RxPin) == LOW;
}

} // namespace orimer::ir

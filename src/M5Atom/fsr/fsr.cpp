#include "fsr_api.h"
#include "fsr400.h"   // 低レベルドライバ
#include <Arduino.h>

namespace orimer::fsr
{

static FSR400* s_fsr = nullptr;
static State   s_state{};
static Mode    s_mode = Mode::None;

// 設定値
static constexpr float PRESS_THRESHOLD = 0.10f;

void Init(uint8_t adcPin)
{
    if (s_fsr) {
        delete s_fsr;
        s_fsr = nullptr;
    }

    s_fsr = new FSR400(adcPin);
    s_fsr->begin();

    s_state = {};
    s_mode  = Mode::Ready;
}

void Update(void)
{
    if (!s_fsr || s_mode != Mode::Ready) {
        return;
    }

    s_state.raw        = s_fsr->readRaw();
    s_state.normalized = s_fsr->readNormalized();
    s_state.force      = s_fsr->readForce();
    s_state.pressed    = (s_state.normalized > PRESS_THRESHOLD);
}

bool GetState(State& state)
{
    if (s_mode != Mode::Ready) {
        return false;
    }

    state = s_state;
    return true;
}

bool IsPressed(void)
{
    if (s_mode != Mode::Ready) {
        return false;
    }
    return s_state.pressed;
}

Mode GetMode(void)
{
    return s_mode;
}

} // namespace orimer::fsr

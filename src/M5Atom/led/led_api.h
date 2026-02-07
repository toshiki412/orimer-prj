#pragma once
#include "led_types.h"

namespace orimer::led {
    void Initialize() noexcept;
    void SetLed(orimer::led::LedColor color) noexcept;
}
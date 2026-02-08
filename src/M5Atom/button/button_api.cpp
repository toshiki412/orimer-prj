#include "button_api.h"

#include <Arduino.h>
#include <M5Atom.h>

namespace orimer::button {

namespace {
    // M5Atom のボタン GPIO
    constexpr uint8_t k_ButtonPin = 39;
}

void Initialize()
{
    // M5.begin() 後で呼ばれる前提
    pinMode(k_ButtonPin, INPUT_PULLUP);
}

bool IsPressed()
{
    // M5Atom のボタンは LOW = 押下
    return digitalRead(k_ButtonPin) == LOW;
}

} // namespace orimer::button

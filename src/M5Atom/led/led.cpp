#include <FastLED.h>
#include "led_types.h"
#include "led_api.h"

namespace orimer::led {
    namespace 
    {
        constexpr uint8_t LedPin = 27;
        constexpr int LedNum = 1;
        CRGB g_Leds[LedNum];
    }

    void Initialize() noexcept
    {
        // LED 初期化
        FastLED.addLeds<WS2812, LedPin, GRB>(g_Leds, LedNum);
        g_Leds[0] = CRGB::Black;
        FastLED.show();
    }

    // LED 制御
    void SetLed(orimer::led::LedColor color) noexcept
    {
        switch (color)
        {
            case LedColor::RED:
                g_Leds[0] = CRGB::Red;
                break;
            case LedColor::Green:
                g_Leds[0] = CRGB::Green;
                break;
            case LedColor::Blue:
                g_Leds[0] = CRGB::Blue;
                break;            
            default:
                break;
        }

        FastLED.show();
    }
}
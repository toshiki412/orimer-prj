#pragma once
#include <Arduino.h>

class FSR400 {
public:
    explicit FSR400(uint8_t adcPin);

    void begin();

    int readRaw();                 // ADC生値
    float readNormalized();        // 0.0 ~ 1.0
    float readForce();             // 擬似荷重（N相当）

private:
    uint8_t _adcPin;
    int _adcMax = 4095;            // ESP32 12bit
};

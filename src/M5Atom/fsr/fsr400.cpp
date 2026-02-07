#include "FSR400.h"

FSR400::FSR400(uint8_t adcPin)
: _adcPin(adcPin)
{
}

void FSR400::begin() {
    analogReadResolution(12);       // 0-4095
    analogSetPinAttenuation(_adcPin, ADC_11db); // 0~3.3V
}

int FSR400::readRaw() {
    return analogRead(_adcPin);
}

float FSR400::readNormalized() {
    int raw = readRaw();
    return (float)raw / _adcMax;
}

/*
 * 擬似荷重計算
 * FSRは非線形なので「それっぽく」する
 */
float FSR400::readForce() {
    int raw = readRaw();

    if (raw < 50) return 0.0f;

    // 電圧に変換
    float v = (float)raw / _adcMax * 3.3;

    // 超ざっくりモデル（実測前提）
    // force ≈ a / R ≈ k * V / (3.3 - V)
    float force = (v / (3.3 - v)) * 10.0; // 10はスケール係数

    return force;
}
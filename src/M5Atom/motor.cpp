#include "motor.h"

Motor::Motor(uint8_t pin0, uint8_t pin1) 
: m_Pin0(pin0), m_Pin1(pin1) 
{
}

void Motor::Initialize(uint8_t pwmChannel0, uint8_t pwmChannel1) 
{
    // 1. ピンをデジタル出力に設定（PWM前）
    pinMode(m_Pin0, OUTPUT);
    pinMode(m_Pin1, OUTPUT);
    digitalWrite(m_Pin0, LOW);  // 強制LOW
    digitalWrite(m_Pin1, LOW);

    m_PwmChannel0 = pwmChannel0;
    m_PwmChannel1 = pwmChannel1;
    
    // PWMチャネル設定
    uint32_t freq        = 25000; // 25kHz
    uint8_t resolution  = 8;    // Duty cycle resolution in bits (8-bit = 0 to 255)

    ledcSetup(m_PwmChannel0, freq, resolution);  // チャネル0、周波数10kHz、8bit
    ledcSetup(m_PwmChannel1, freq, resolution);  // チャネル1
    ledcAttachPin(m_Pin0, m_PwmChannel0);  // ピンをチャネルに割り当て
    ledcAttachPin(m_Pin1, m_PwmChannel1);

    this->Stop();
}

void Motor::Idling() 
{
    Serial.println("Motor Idling");
    ledcWrite(m_PwmChannel0, 0);
    ledcWrite(m_PwmChannel1, 0);
}

void Motor::Stop() 
{
    Serial.println("Motor Stop");
    ledcWrite(m_PwmChannel0, 255);
    ledcWrite(m_PwmChannel1, 255);
}

void Motor::Forward(float inputVol) {
    Serial.println("Motor Forward");
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    ledcWrite(m_PwmChannel0, 0);
    ledcWrite(m_PwmChannel1, static_cast<int>(inputVol * 255));
    delayMicroseconds(100);
}

void Motor::Backward(float inputVol) {
    Serial.println("Motor Backward");
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    ledcWrite(m_PwmChannel0, static_cast<int>(inputVol * 255));
    ledcWrite(m_PwmChannel1, 0);
    delayMicroseconds(100);
}


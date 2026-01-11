#include "motor.h"

Motor::Motor(int pin0, int pin1) 
: m_Pin0(pin0), m_Pin1(pin1) 
{
}

void Motor::Initialize(int pwmChannel0, int pwmChannel1) 
{
    m_PwmChannel0 = pwmChannel0;
    m_PwmChannel1 = pwmChannel1;
    
    // PWMチャネル設定
    int freq        = 10000; // 10kHz
    int resolution  = 8;    // Duty cycle resolution in bits (8-bit = 0 to 255)

    ledcSetup(m_PwmChannel0, freq, resolution);  // チャネル0、周波数10kHz、8bit
    ledcSetup(m_PwmChannel1, freq, resolution);  // チャネル1
    ledcAttachPin(m_Pin0, m_PwmChannel0);  // ピンをチャネルに割り当て
    ledcAttachPin(m_Pin1, m_PwmChannel1);

    this->Idling();
}

void Motor::Idling() 
{
    Serial.println("Motor Idling");
    ledcWrite(m_PwmChannel0, 0);  // analogWrite → ledcWrite
    ledcWrite(m_PwmChannel1, 0);
}

void Motor::Forward(float inputVol) {
    Serial.println("Motor Forward");
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    ledcWrite(m_PwmChannel0, 0);
    ledcWrite(m_PwmChannel1, static_cast<int>(inputVol * 255));
}

void Motor::Backward(float inputVol) {
    Serial.println("Motor Idling");
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    ledcWrite(m_PwmChannel0, static_cast<int>(inputVol * 255));
    ledcWrite(m_PwmChannel1, 0);
}


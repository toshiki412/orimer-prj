#include "motor.h"
#include <Arduino.h>

namespace orimer::motor
{

namespace
{
constexpr uint32_t kPwmFreq = 25000;
constexpr uint8_t  kPwmResolution = 8;
constexpr uint8_t  kMaxDuty = 255;
}

Motor::Motor(uint8_t pin0, uint8_t pin1)
    : m_Pin0(pin0),
      m_Pin1(pin1),
      m_PwmChannel0(0),
      m_PwmChannel1(0)
{
}

void Motor::Initialize(uint8_t pwmChannel0, uint8_t pwmChannel1)
{
    pinMode(m_Pin0, OUTPUT);
    pinMode(m_Pin1, OUTPUT);

    digitalWrite(m_Pin0, LOW);
    digitalWrite(m_Pin1, LOW);

    m_PwmChannel0 = pwmChannel0;
    m_PwmChannel1 = pwmChannel1;

    ledcSetup(m_PwmChannel0, kPwmFreq, kPwmResolution);
    ledcSetup(m_PwmChannel1, kPwmFreq, kPwmResolution);

    ledcAttachPin(m_Pin0, m_PwmChannel0);
    ledcAttachPin(m_Pin1, m_PwmChannel1);

    Stop();

    Serial.println("[Motor] Initialized");
}

void Motor::Idling()
{
    ledcWrite(m_PwmChannel0, 0);
    ledcWrite(m_PwmChannel1, 0);
}

void Motor::Stop()
{
    // 安全停止 = PWMゼロ
    Idling();
}

void Motor::Brake()
{
    // 強制ブレーキ
    ledcWrite(m_PwmChannel0, kMaxDuty);
    ledcWrite(m_PwmChannel1, kMaxDuty);
}

void Motor::Forward(float duty)
{
    duty = constrain(duty, 0.0f, 1.0f);

    ledcWrite(m_PwmChannel0, 0);
    ledcWrite(m_PwmChannel1, static_cast<uint8_t>(duty * kMaxDuty));
}

void Motor::Backward(float duty)
{
    duty = constrain(duty, 0.0f, 1.0f);

    ledcWrite(m_PwmChannel0, static_cast<uint8_t>(duty * kMaxDuty));
    ledcWrite(m_PwmChannel1, 0);
}

void Motor::SetSpeed(float value)
{
    value = constrain(value, -1.0f, 1.0f);

    if (value > 0)
    {
        Forward(value);
    }
    else if (value < 0)
    {
        Backward(-value);
    }
    else
    {
        Stop();
    }
}
} // namespace orimer::motor
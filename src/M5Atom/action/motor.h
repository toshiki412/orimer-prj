#pragma once
#include <cstdint>
namespace orimer::motor
{

class Motor
{
public:
    Motor(uint8_t pin0, uint8_t pin1);

    void Initialize(uint8_t pwmChannel0, uint8_t pwmChannel1);

    void Forward(float duty);
    void Backward(float duty);

    void Stop();     // 安全停止
    void Brake();    // 強制ブレーキ
    void Idling();   // PWMゼロ

    void SetSpeed(float value); // -1.0 ~ 1.0

private:
    uint8_t m_Pin0;
    uint8_t m_Pin1;

    uint8_t m_PwmChannel0;
    uint8_t m_PwmChannel1;
};

}


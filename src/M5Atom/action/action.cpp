#include "action.h"

#include <Arduino.h>
#include <math.h>

namespace orimer::action
{

namespace
{
constexpr float kLearningRate = 0.2f;

constexpr float kKW = 0.5f;
constexpr float kKO = 0.5f;
constexpr float kKT = 0.16f;

constexpr float kCoefficient = 286.4788976f;
constexpr float kWheelRadius = 0.1f;
constexpr float kWheelWidth  = 0.01f;

constexpr float kAlpha = 0.5f;
constexpr float kIdleThreshold = 0.05f;
}

Action::Action()
    :
    m_MotorL(MOTOR_L_PIN0, MOTOR_L_PIN1),
    m_MotorR(MOTOR_R_PIN0, MOTOR_R_PIN1),
    m_TargetW(0.0f),
    m_CurrentW(0.0f)
{
}

void Action::Initialize()
{
    m_TargetW = 0.0f;
    m_CurrentW = 0.0f;

    m_MotorL.Initialize(MOTOR_L_CH0, MOTOR_L_CH1);
    m_MotorR.Initialize(MOTOR_R_CH0, MOTOR_R_CH1);

    Serial.println("[ACTION] Initialized");
}

void Action::Finalize()
{
    m_MotorL.Stop();
    m_MotorR.Stop();

    Serial.println("[ACTION] Finalized");
}

void Action::SetTargetAngular(float targetW)
{
    m_TargetW = targetW;
}

void Action::LineControl()
{
    m_CurrentW =
        m_CurrentW
        - (kKW * m_CurrentW - kKO * m_TargetW)
        * kKT;
}

void Action::AutoMoving(float targetV)
{
    const float left =
        +kCoefficient *
        (targetV - m_CurrentW * kWheelWidth)
        / kWheelRadius;

    const float right =
        -kCoefficient *
        (targetV + m_CurrentW * kWheelWidth)
        / kWheelRadius;

    Serial.printf(
        "[ACTION] Auto L=%.2f R=%.2f\n",
        left,
        right
    );

    m_MotorL.SetSpeed(left);
    m_MotorR.SetSpeed(right);
}

void Action::ManualMoving(float targetV, float targetW)
{
    if (targetV == 0.0f && targetW == 0.0f)
    {
        Serial.println("[ACTION] Stop");
        m_MotorL.Stop();
        m_MotorR.Stop();
        return;
    }

    float left =
        fabsf(targetV + kAlpha * targetW);

    float right =
        fabsf(targetV - kAlpha * targetW);

    left  /= (1.0f + kAlpha);
    right /= (1.0f + kAlpha);

    Serial.printf(
        "[ACTION] Manual L=%.2f R=%.2f\n",
        left,
        right
    );

    if (left < kIdleThreshold && right < kIdleThreshold)
    {
        m_MotorL.Idling();
        m_MotorR.Idling();
        return;
    }

    if (targetV > 0)
    {
        m_MotorL.Backward(left);
        m_MotorR.Forward(right);
    }
    else if (targetV < 0)
    {
        m_MotorL.Forward(left);
        m_MotorR.Backward(right);
    }
    else
    {
        m_MotorL.Idling();
        m_MotorR.Idling();
    }
}

void Action::Odometry()
{
    // TODO: encoder integration
}

} // namespace orimer::action

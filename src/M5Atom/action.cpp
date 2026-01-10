#include <iostream>
#include <cmath>
#include "action.h"


constexpr double RATE = 0.2; //学習率
constexpr double KW = 0.5;
constexpr double KO = 0.5;
constexpr double KT = 0.16; 
constexpr double COEFFICIENT = 286.4788976; //rad/s -> rpm
constexpr double WHEEL = 0.1; //車輪半径[m]
constexpr double ROBO_W = 0.01; //車輪間幅[m]


ACTION::ACTION()
    : m_MotorL(MOTOR_L_PIN0, MOTOR_L_PIN1),
      m_MotorR(MOTOR_R_PIN0, MOTOR_R_PIN1)
{
}

void ACTION::Initialize()
{
    m_tO = 0.0;
    m_MTnW = 0.0;
    m_MToW = 0.0;
}

void ACTION::Finalize()
{
    // Do Nothong
}

void ACTION::SetTargetO(double tO) 
{
    m_tO = tO;
}

void ACTION::LineControl() 
{
    m_MTnW = m_MTnW - (KW * m_MTnW - KO * m_tO) * KT;
}

void ACTION::AutoMoving(double output_v) 
{
    int LnW = static_cast<int>(+COEFFICIENT * (output_v - m_MTnW * ROBO_W) / WHEEL);
    int RnW = static_cast<int>(-COEFFICIENT * (output_v + m_MTnW * ROBO_W) / WHEEL);
    std::cout << LnW << " " << RnW << std::endl;
}

void ACTION::ManualMoving(float targetV, float targetW) 
{
    constexpr float ALPHA = 0.5f; // 旋回時の速度調整係数
    
    float inputVolLeft  = fabs(targetV + ALPHA * targetW);
    float inputVolRight = fabs(targetV - ALPHA * targetW);

    inputVolLeft  /= (1.0f + ALPHA);
    inputVolRight /= (1.0f + ALPHA);

    if (inputVolLeft < 0.05f || inputVolRight < 0.05f) {
        m_MotorL.Idling();
        m_MotorR.Idling();
        return;
    }

    if (targetV > 0) {
        m_MotorL.Backward(inputVolLeft);
        m_MotorR.Forward(inputVolRight);
    } else if (targetV < 0) {
        m_MotorL.Forward(inputVolLeft);
        m_MotorR.Backward(inputVolRight);
    } else {
        m_MotorL.Idling();
        m_MotorR.Idling();
    }

}

void ACTION::Odometory() 
{
    //Do Nothing
}
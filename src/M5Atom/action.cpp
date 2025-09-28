#include <iostream>
#include <cmath>
#include "action.h"


constexpr double RATE = 0.2; //学習率
constexpr double KW = 0.5;
constexpr double KO = 0.5;
constexpr double KT = 0.16; 
constexpr double COEFFICIENT = 286.4788976; //rad/s -> rpm
constexpr double WHEEL = 0.1; //車輪半径[m]
constexpr double ROBO_W = 0.01; //車輪間幅

ACTION::ACTION()
{
    //Do Nothing
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

void ACTION::Moving(double output_v) 
{
    int LnW = static_cast<int>(+COEFFICIENT * (output_v - m_MTnW * ROBO_W) / WHEEL);
    int RnW = static_cast<int>(-COEFFICIENT * (output_v + m_MTnW * ROBO_W) / WHEEL);
    std::cout << LnW << " " << RnW << std::endl;
}

void Odometory() 
{
    //Do Nothing
}
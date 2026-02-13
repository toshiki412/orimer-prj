 /******************************************************************************
 * Copyright (c) 2025 [orimer]. All rights reserved.
 *
 * Unauthorized reproduction or distribution of this source code is prohibited.
 * No part of this code may be copied, reproduced, or distributed without
 * the prior written permission of the copyright holder.
 ******************************************************************************/

#include <iostream>
#include <cmath>
#include "motor.h"

// PWMピン
#define MOTOR_L_PIN0 25 // BIN2
#define MOTOR_L_PIN1 21 // BIN1
#define MOTOR_R_PIN0 19 // AIN2
#define MOTOR_R_PIN1 22 // AIN1

#define MOTOR_L_CH0 2
#define MOTOR_L_CH1 3  
#define MOTOR_R_CH0 4
#define MOTOR_R_CH1 5

#pragma once

#include "motor.h"

namespace orimer::action {

class Action
{
public:
    Action();

    void Initialize();
    void Finalize();

    // 制御入力
    void SetTargetAngular(float targetW);

    // 制御処理
    void LineControl();
    void AutoMoving(float targetV);
    void ManualMoving(float targetV, float targetW);

    void Odometry();

private:
    motor::Motor m_MotorL;
    motor::Motor m_MotorR;

    float m_TargetW;     // target angular velocity
    float m_CurrentW;    // current angular velocity
};

} // namespace orimer::action


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
#define MOTOR_L_PIN0 24
#define MOTOR_L_PIN1 25
#define MOTOR_R_PIN0 22
#define MOTOR_R_PIN1 23

class ACTION {
private:
    double m_tO;
    double m_MTnW;
    double m_MToW;
    Motor m_MotorL;
    Motor m_MotorR;

public:
    ACTION();
    void Initialize();
    void Finalize();
    void SetTargetO(double tO);
    void LineControl();
    void AutoMoving(double output_v);
    void ManualMoving(float targetV, float targetW);
    void Odometory();
};

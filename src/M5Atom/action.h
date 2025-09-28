 /******************************************************************************
 * Copyright (c) 2025 [orimer]. All rights reserved.
 *
 * Unauthorized reproduction or distribution of this source code is prohibited.
 * No part of this code may be copied, reproduced, or distributed without
 * the prior written permission of the copyright holder.
 ******************************************************************************/

#include <iostream>
#include <cmath>

class ACTION {
private:
    double m_tO;
    double m_MTnW;
    double m_MToW;

public:
    ACTION();
    void Initialize();
    void Finalize();
    void SetTargetO(double tO);
    void LineControl();
    void Moving(double output_v);
    void Odometory();
};

#pragma once
#include <stdint.h>

namespace orimer::ir {

class IrDiodeDriver
{
public:
    explicit IrDiodeDriver(int pin);

    void Begin();
    void StartCarrier();
    void StopCarrier();
    void SendBurst(uint16_t durationMs);

private:
    int m_pin;
    int m_pwmChannel = 0;
};

}

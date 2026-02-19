#include "ir_diode_driver.h"
#include <Arduino.h>

namespace orimer::ir {

IrDiodeDriver::IrDiodeDriver(int pin)
    : m_pin(pin)
{
}

void IrDiodeDriver::Begin()
{
    ledcSetup(m_pwmChannel, 38000, 8);  // 38kHz, 8bit
    ledcAttachPin(m_pin, m_pwmChannel);
}

void IrDiodeDriver::StartCarrier()
{
    ledcWrite(m_pwmChannel, 128);  // 50% duty
}

void IrDiodeDriver::StopCarrier()
{
    ledcWrite(m_pwmChannel, 0);
}

void IrDiodeDriver::SendBurst(uint16_t durationMs)
{
    StartCarrier();
    delay(durationMs);
    StopCarrier();
}

}

#include "ir_VS1838B_driver.h"
#include <Arduino.h>

namespace orimer::ir {

IrVs1838bDriver::IrVs1838bDriver(int pin)
    : m_pin(pin)
{
}

void IrVs1838bDriver::Begin()
{
    pinMode(m_pin, INPUT);
}

bool IrVs1838bDriver::IsReceiving() const
{
    // VS1838BはアクティブLOW
    return digitalRead(m_pin) == LOW;
}

int IrVs1838bDriver::ReadRaw() const
{
    return digitalRead(m_pin);
}

}

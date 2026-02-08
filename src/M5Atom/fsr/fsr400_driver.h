#pragma once

#include <Arduino.h>          // ★ uint8_t / pinMode / Serial
#include <stdint.h>           // ★ uint16_t
#include "fsr_types.h"        // ★ FsrStat

namespace orimer::fsr {

/**
 * @brief FSR-400 Driver
 */
class FsrDriver
{
public:
    static FsrDriver* GetInstance()
    {
        static FsrDriver instance;
        return &instance;
    }

    void Initialize();
    void Update();
    bool GetState(FsrState& state) const;
    bool IsInitialized() const;

private:
    FsrDriver();
    float ConvertToForce(uint16_t raw) const;

private:
    uint8_t  m_AdcPin;
    bool     m_Initialized;
    FsrState m_State;
};

}
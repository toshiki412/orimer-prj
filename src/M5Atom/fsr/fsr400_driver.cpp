#include "fsr400_driver.h"

namespace orimer::fsr {

namespace
{
    constexpr float k_AdcMax     = 4095.0f;
    constexpr float k_RefVoltage = 3.3f;
    constexpr uint8_t AdcPin = 32;
}

    FsrDriver::FsrDriver()
        : m_AdcPin(0)
        , m_Initialized(false)
    {
        m_State = {};
    }

    float FsrDriver::ConvertToForce(uint16_t raw) const
    {
        // NOTE: 非線形・仮実装（後でキャリブレーション前提）
        if (raw < 50)
        {
            return 0.0f;
        }

        return static_cast<float>(raw);
    }

    void FsrDriver::Initialize()
    {
        m_AdcPin = AdcPin;
        pinMode(m_AdcPin, INPUT);
        m_Initialized = true;

        Serial.printf(
            "[FSR] Init adcPin=%d\n",
            m_AdcPin
        );
    }

    void FsrDriver::Update()
    {
        if (!m_Initialized)
        {
            return;
        }

        const uint16_t raw = analogRead(m_AdcPin);

        m_State.raw     = raw;
        m_State.voltage = (raw / k_AdcMax) * k_RefVoltage;
        m_State.force   = ConvertToForce(raw);
    }

    bool FsrDriver::GetState(FsrState& state) const
    {
        if (!m_Initialized)
        {
            return false;
        }

        state = m_State;
        return true;
    }

    bool FsrDriver::IsInitialized() const
    {
        return m_Initialized;
    }
}
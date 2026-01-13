#include <Arduino.h>
class Motor {

public:
    Motor(uint8_t pin0, uint8_t pin1);
    void Initialize(uint8_t pwmChannel0, uint8_t pwmChannel1);
    void Idling();
    void Stop();
    void Forward(float inputVol);
    void Backward(float inputVol);

private:
    uint8_t m_Pin0;
    uint8_t m_Pin1;

    uint8_t m_PwmChannel0;
    uint8_t m_PwmChannel1;
};

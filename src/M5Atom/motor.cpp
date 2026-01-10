#include "motor.h"

Motor::Motor(int pin0, int pin1) 
: m_Pin0(pin0), m_Pin1(pin1) 
{
    pinMode(m_Pin0, OUTPUT);
    pinMode(m_Pin1, OUTPUT);
    analogWrite(m_Pin0, 0);
    analogWrite(m_Pin1, 0);
}

void Motor::Idling() {
        analogWrite(m_Pin0, 0);
        analogWrite(m_Pin1, 0);
    }

void Motor::Forward(float inputVol) {
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    analogWrite(m_Pin0, 0);
    analogWrite(m_Pin1, int(inputVol * 255));
}

void Motor::Backward(float inputVol) {
    inputVol = constrain(inputVol, 0.0f, 1.0f);
    analogWrite(m_Pin0, int(inputVol * 255));
    analogWrite(m_Pin1, 0);
}


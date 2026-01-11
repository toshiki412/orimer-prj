#include <Arduino.h>

// // モーター制御用ピン
// const int MOTOR1_PIN = 22;
// const int MOTOR2_PIN = 23;

// void moveMotor(char cmd, double val) {
//   switch(cmd) {
//     case 'F':
//       digitalWrite(MOTOR1_PIN, val); // HIGH LOW を float にする
//       digitalWrite(MOTOR2_PIN, val);
//       break;
//     case 'B':
//       digitalWrite(MOTOR1_PIN, val);
//       digitalWrite(MOTOR2_PIN, val);
//       break;
//     case 'L':
//       // 旋回処理（左右モーターの片方だけ回す）
//       digitalWrite(MOTOR1_PIN, val);    // 左モーター
//       digitalWrite(MOTOR2_PIN, LOW);    // 右モーターは停止
//       break;
//     case 'R':
//       // 逆旋回処理
//       digitalWrite(MOTOR1_PIN, LOW);    // 左モーターは停止
//       digitalWrite(MOTOR2_PIN, val);    // 右モーター
//       break;
//   }
// }
class Motor {
public:
    Motor(int pin0, int pin1);
    void Initialize(int pwmChannel0, int pwmChannel1);
    void Idling();
    void Forward(float inputVol);
    void Backward(float inputVol);

private:
    int m_Pin0;
    int m_Pin1;

    int m_PwmChannel0;
    int m_PwmChannel1;
};

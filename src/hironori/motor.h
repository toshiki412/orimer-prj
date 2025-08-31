#include <Arduino.h>

// モーター制御用ピン
const int MOTOR1_PIN = 22;
const int MOTOR2_PIN = 23;

void moveMotor(char cmd, double val) {
  switch(cmd) {
    case 'F':
      digitalWrite(MOTOR1_PIN, val); // HIGH LOW を float にする
      digitalWrite(MOTOR2_PIN, val);
      break;
    case 'B':
      digitalWrite(MOTOR1_PIN, val);
      digitalWrite(MOTOR2_PIN, val);
      break;
    case 'L':
      // 旋回処理（左右モーターの片方だけ回す）
      digitalWrite(MOTOR1_PIN, val);    // 左モーター
      digitalWrite(MOTOR2_PIN, LOW);    // 右モーターは停止
      break;
    case 'R':
      // 逆旋回処理
      digitalWrite(MOTOR1_PIN, LOW);    // 左モーターは停止
      digitalWrite(MOTOR2_PIN, val);    // 右モーター
      break;
  }
}

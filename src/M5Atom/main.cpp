// controller の接続方法
// コントローラー上部の sync ボタンでペアリングした後、 home + Y 長押しで接続済みになる
// 接続したら、procon_ble.pyを実行する
#include <M5Atom.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "esp_bt_device.h"
#include "motor.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool ledState = false;

void printBluetoothMacAddress() {
  const uint8_t* mac = esp_bt_dev_get_address();
  Serial.print("Bluetooth MAC Address: ");
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
      printBluetoothMacAddress();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
      std::string rxValue = pChar->getValue();
      if (rxValue.length() > 0) {
        // Serial.print("Received Value: ");
        // Serial.println(rxValue.c_str());

        if (rxValue == "X") {
          M5.dis.drawpix(0, CRGB(0, 255, 0)); // LED ON 緑
          ledState = true;
        } else if (rxValue == "Q") {
          M5.dis.drawpix(0, CRGB(0, 0, 0));   // LED OFF
          ledState = false;
        } else  {
          char command = rxValue[0];
          moveMotor(command, 0.5);
        }
      }
    }
};

void setup() {
  // M5.begin(false, false, true);
  M5.begin(true, false, true);
  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);

  Serial.begin(115200);
  delay(5000);
  
  BLEDevice::init("M5Atom-BLE");
  
  printBluetoothMacAddress();
  
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE service started, waiting for client to connect...");
}

void loop() {
  // 特に何もしなくてもOK
  // delay(1000);
}

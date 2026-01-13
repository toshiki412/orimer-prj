// controller の接続方法
// コントローラー上部の sync ボタンでペアリングした後、 home + Y 長押しで接続済みになる
// 接続したら、procon_ble.pyを実行する
#include <M5Atom.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "esp_bt_device.h"
#include "action.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

namespace {

bool g_IsDeviceConnected = false;
bool ledState = false;

BLECharacteristic *g_pCharacteristic;
BLEService *g_pService;
BLEAdvertising *g_pAdvertising;
ACTION g_Action;

void PrintLedcStatusForDebug() 
{
	Serial.println("--- Motor Status ---");
	Serial.printf("L: CH%d=%d CH%d=%d | R: CH%d=%d CH%d=%d\n", 
		2, ledcRead(2), 3, ledcRead(3), 4, ledcRead(4), 5, ledcRead(5));
	Serial.printf("GPIO33電圧: %f\n", analogRead(33));
	Serial.printf("GPIO23電圧: %f\n", analogRead(23));     
	Serial.printf("GPIO22電圧: %f\n", analogRead(22));     
	Serial.printf("GPIO19電圧: %f\n", analogRead(19));     
}

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
      g_IsDeviceConnected = true;
      Serial.println("Device connected");
      printBluetoothMacAddress();
    };

    void onDisconnect(BLEServer* pServer) {
      g_IsDeviceConnected = false;
      Serial.println("Device disconnected");
    }
};

void FinalizeBLE() 
{
    if (g_IsDeviceConnected) {
      g_IsDeviceConnected = false;
    }

    g_pAdvertising->stop();
    g_pAdvertising->start();
    Serial.println("BLE service stopped and cleaned up.");
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
      std::string rxValue = pChar->getValue();
      if (rxValue.length() > 0) {
        Serial.printf("Received Value: %s\n", rxValue.c_str());
        Serial.printf("Received Value Length: %d\n", rxValue.length());

        if (rxValue == "X") {
        //   M5.dis.drawpix(0, CRGB(0, 255, 0)); // LED ON 緑
          g_Action.ManualMoving(1.0, 0.0);
          ledState = true;
        } else if (rxValue == "Q") {
        //   M5.dis.drawpix(0, CRGB(0, 0, 0));   // LED OFF
          g_Action.ManualMoving(0.0, 0.0);
          ledState = false;
        } else if (rxValue == "E") {
        //   M5.dis.drawpix(0, CRGB(0, 0, 0));   // LED OFF
          FinalizeBLE();
		  g_Action.ManualMoving(0.0, 0.0);
        } else  {
            float targetV = 0.5f;
            if (rxValue.length() >= 1) {
                char command = rxValue[0];
                Serial.printf("Command: '%c'\n", command);
                
                float targetW = 0.0f;
                if (rxValue.length() > 1) {
                    std::string numStr = rxValue.substr(1);
                    Serial.printf("Number string: '%s'\n", numStr.c_str());
                    try {
                        targetW = std::stof(numStr);
                        Serial.printf("Parsed targetW: %.2f\n", targetW);
                    } catch (...) {
                        Serial.println("Parse failed");
                    }
                }
                Serial.printf("Moving V=%.2f, W=%.2f\n", targetV, targetW);
                g_Action.ManualMoving(targetV, targetW);
            }
        }
      }
    }
};

void InitializeBLE() 
{
    BLEDevice::init("M5Atom-BLE");
  
    printBluetoothMacAddress();
  
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    g_pService = pServer->createService(SERVICE_UUID);

    g_pCharacteristic = g_pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE
                      );

    g_pCharacteristic->setCallbacks(new MyCallbacks());
    g_pCharacteristic->setValue("Hello World");
    g_pService->start();

    g_pAdvertising = BLEDevice::getAdvertising();
    g_pAdvertising->addServiceUUID(SERVICE_UUID);
    g_pAdvertising->setScanResponse(true);
    g_pAdvertising->start();

    Serial.println("BLE service started, waiting for client to connect...");
}

void InitializeWiFi() 
{
    #define SSID        ""
    #define PASSWORD    ""
    WiFi.begin(SSID, PASSWORD);
    // 接続完了まで待機
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected.");
}

void FinalizeWiFi() 
{
}

void InitializeMotor()
{
	g_Action.Initialize();
}

void FinalizeMotor()
{
	g_Action.Finalize();
}
} // namespace

void setup() 
{
	Serial.begin(115200);
	delay(5000);

	M5.begin(true, true, true);
	InitializeMotor();
	InitializeBLE();
	InitializeWiFi();
}

void loop() 
{
	// 1s間隔でLEDCの状態を表示
	static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 1000) 
	{
		PrintLedcStatusForDebug();
		lastCheck = millis();
	}
}

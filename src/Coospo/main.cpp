#ifdef COOSPO
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <FastLED.h>

// =====================
// LED 設定
// =====================
#define LED_PIN 27
#define LED_NUM 1
CRGB leds[LED_NUM];

// =====================
// BLE UUID（PCで流れてたやつ）
// =====================
static BLEUUID HR_CHAR_UUID("04ca1581-fd57-404e-8459-c5ef8d765c8d");

// =====================
// 対象デバイス（HW9）
// =====================
#define TARGET_ADDR "f0:d6:b5:f6:f0:48"  // 小文字推奨

BLEAdvertisedDevice* targetDevice = nullptr;
BLERemoteCharacteristic* hrChar = nullptr;
BLEClient* client = nullptr;
bool connected = false;

// =====================
// LED 制御
// =====================
void setLedByPercent(uint8_t percent) {
  if (percent < 60) {
    leds[0] = CRGB::Red;
  } else {
    leds[0] = CRGB::Blue;
  }
  FastLED.show();
}

// =====================
// notify コールバック
// =====================
void notifyCallback(
  BLERemoteCharacteristic*,
  uint8_t* data,
  size_t length,
  bool
) {
  Serial.printf("notify len=%d RAW: ", length);
  for (size_t i = 0; i < length; i++) {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();

  if (length < 13) return;

  /*
    観測結果より：
    data[10] : 0xCF〜0xD3 あたりで変動
    → 心拍ゾーン(%)を 256 スケールで持ってる可能性が高い
  */
  uint8_t zoneRaw = data[10];     // 0–255
  float zoneRatio = zoneRaw / 256.0f;
  uint8_t percent = (uint8_t)(zoneRatio * 100.0f);

  Serial.printf("Zone = %d %%\n", percent);

  setLedByPercent(percent);
}

// =====================
// スキャンコールバック
// =====================
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    std::string addr = advertisedDevice.getAddress().toString();
    std::transform(addr.begin(), addr.end(), addr.begin(), ::tolower);

    if (addr == TARGET_ADDR) {
      Serial.println("Found target device!");
      targetDevice = new BLEAdvertisedDevice(advertisedDevice);
      BLEDevice::getScan()->stop();
    }
  }
};

// =====================
// setup
// =====================
void setup() {
  Serial.begin(115200);

  // LED 初期化
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  leds[0] = CRGB::Black;
  FastLED.show();

  // BLE 初期化
  BLEDevice::init("");
  BLEScan* scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setActiveScan(true);
  scan->start(0);

  Serial.println("Scanning...");
}

// =====================
// loop
// =====================
void loop() {
  if (targetDevice && !connected) {
    Serial.println("Connecting...");

    client = BLEDevice::createClient();
    if (!client->connect(targetDevice)) {
      Serial.println("Connect failed");
      return;
    }

    Serial.println("Connected");

    // サービス走査
    auto services = client->getServices();
    for (auto& s : *services) {
      if (s.second->getCharacteristic(HR_CHAR_UUID)) {
        hrChar = s.second->getCharacteristic(HR_CHAR_UUID);
        break;
      }
    }

    if (!hrChar) {
      Serial.println("HR characteristic not found");
      return;
    }

    if (hrChar->canNotify()) {
      hrChar->registerForNotify(notifyCallback);

      // ★ CCCD を必ず書く ★
      BLERemoteDescriptor* cccd =
        hrChar->getDescriptor(BLEUUID((uint16_t)0x2902));

      if (cccd) {
        uint8_t notifyOn[] = {0x01, 0x00};
        cccd->writeValue(notifyOn, 2, true);
        Serial.println("CCCD written (notify enabled)");
      } else {
        Serial.println("CCCD not found");
      }

      connected = true;
      Serial.println("Subscribed to HR notify");
    }
  }

  vTaskDelay(pdMS_TO_TICKS(100));
}


#endif // COOSPO
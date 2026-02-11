#include "camera_driver.h"

namespace orimer::camera
{

namespace
{
    // AtomS3R CAM ピン定義
    constexpr int PWDN_GPIO_NUM     = -1;
    constexpr int RESET_GPIO_NUM    = -1;
    constexpr int XCLK_GPIO_NUM     = 10;
    constexpr int SIOD_GPIO_NUM     = 40;
    constexpr int SIOC_GPIO_NUM     = 39;

    constexpr int Y9_GPIO_NUM       = 48;
    constexpr int Y8_GPIO_NUM       = 11;
    constexpr int Y7_GPIO_NUM       = 12;
    constexpr int Y6_GPIO_NUM       = 14;
    constexpr int Y5_GPIO_NUM       = 16;
    constexpr int Y4_GPIO_NUM       = 18;
    constexpr int Y3_GPIO_NUM       = 17;
    constexpr int Y2_GPIO_NUM       = 15;
    constexpr int VSYNC_GPIO_NUM    = 38;
    constexpr int HREF_GPIO_NUM     = 47;
    constexpr int PCLK_GPIO_NUM     = 13;
}

CameraDriver::CameraDriver()
{
    m_FrameInfo = {};
}

void CameraDriver::Initialize()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_RGB565;
    config.frame_size   = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count     = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed\n");
        return;
    }

    Serial.println("Camera initialized");
}



} // namespace orimer::camera

#include "audio_api.h"
#include "esp_log.h"
#include <driver/i2s.h>
#include <M5Atom.h>

namespace orimer::audio {

void Initialize()
{
    // アンプ有効
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 19,
        .ws_io_num = 33,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, nullptr);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}


void Beep(BeepTone tone)
{
    static int16_t buffer[1024];
    uint32_t freq = 0, duration_ms = 0;

    switch (tone) {
        case BeepTone::Short: freq = 2000; duration_ms = 100; break;
        case BeepTone::Long:  freq = 1500; duration_ms = 400; break;
        case BeepTone::Error: freq = 500;  duration_ms = 600; break;
    }

    const uint32_t sample_rate = 16000;
    const uint32_t samples = (sample_rate * duration_ms) / 1000;

    for (uint32_t i = 0; i < samples; i++) {
        float t = (float)i / sample_rate;
        buffer[i % 1024] = (int16_t)(sinf(2.0f * M_PI * freq * t) * 12000);

        if ((i % 1024) == 1023) {
            size_t written;
            i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &written, portMAX_DELAY);
        }
    }

    memset(buffer, 0, sizeof(buffer));
    size_t written;
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &written, portMAX_DELAY);
}


} // namespace orimer::audio
#include <Arduino.h>
#include <algorithm>  // std::max
#include "camera_detector.h"
#include "esp_camera.h"

namespace orimer::camera {
namespace {
// ====== 設定 ======
#define RED_THRESHOLD_R 200
#define RED_THRESHOLD_G 80
#define RED_THRESHOLD_B 80
#define RED_DOMINANCE   100
#define MIN_AREA        30   // ノイズ除去

// ===== RGB565 → 8bit =====
inline uint8_t R8(uint16_t c) { return ((c >> 11) & 0x1F) << 3; }
inline uint8_t G8(uint16_t c) { return ((c >> 5)  & 0x3F) << 2; }
inline uint8_t B8(uint16_t c) { return ( c        & 0x1F) << 3; }

bool IsRed(uint8_t r, uint8_t g, uint8_t b)
{
    if (r < RED_THRESHOLD_R) return false;
    if (g > RED_THRESHOLD_G) return false;
    if (b > RED_THRESHOLD_B) return false;
    if ((r - max(g,b)) < RED_DOMINANCE) return false;
    return true;
}
} // namespace

bool DetectRedBlob(orimer::camera::Blob* pOutBlob)
{
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) return false;

    uint16_t* img = (uint16_t*)fb->buf;

    long sumX = 0;
    long sumY = 0;
    int  count = 0;

    for (int y = 0; y < fb->height; y++)
    {
        for (int x = 0; x < fb->width; x++)
        {
            uint16_t pix = img[y * fb->width + x];

            uint8_t r = R8(pix);
            uint8_t g = G8(pix);
            uint8_t b = B8(pix);

            if (IsRed(r, g, b))
            {
                sumX += x;
                sumY += y;
                count++;
            }
        }
    }

    esp_camera_fb_return(fb);

    if (count < MIN_AREA)
        return false;

    pOutBlob->x = sumX / count;
    pOutBlob->y = sumY / count;
    pOutBlob->area = count;

    return true;
}
} // namespace orimer::camera
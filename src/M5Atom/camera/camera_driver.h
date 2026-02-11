#pragma once

#include <Arduino.h>
#include <esp_camera.h>

namespace orimer::camera
{

struct FrameInfo
{
    bool     valid;
    size_t   length;
    uint16_t width;
    uint16_t height;
};

class CameraDriver
{
public:
    CameraDriver();

    void Initialize();

private:
    FrameInfo m_FrameInfo;
    uint8_t* m_pFrameBuffer{nullptr};
};

} // namespace orimer::camera

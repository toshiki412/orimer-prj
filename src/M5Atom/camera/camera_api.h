#pragma once
#include "camera_driver.h"
#include "camera_detector.h"

namespace orimer::camera
{
    void Initialize();
    void Finalize();
    bool DetectRobot(Blob* pOutBlob);
}
#include "camera_api.h"

namespace orimer::camera
{
    CameraDriver g_CameraDriver;

    void Initialize()
    {
        g_CameraDriver.Initialize();
    }

    void Finalize()
    {
    }

    bool DetectRobot(Blob* pOutBlob)
    {
        return camera::DetectRedBlob(pOutBlob);
    }
}
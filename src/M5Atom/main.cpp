#include <M5Atom.h>
#include "config.h"

#include "ble/ble_api.h"
#include "ble/ble_types.h"

#include "led/led_api.h"
#include "led/led_types.h"

#include "button/button_api.h"

#if defined(ORIMER_HUEDEVICE)
    #include "audio/audio_api.h"
    #include "audio/audio_types.h"
    #include "fsr/fsr_types.h"
    #include "fsr/fsr_api.h"
#endif // defined(ORIMER_HUEDEVICE)

#if defined(ORIMER_PIKMIN)
    #include "action/action_api.h"
#endif // defined(ORIMER_PIKMIN)

#if defined(USE_CAMERA)
    #include "camera/camera_api.h"
    #include "camera/camera_types.h"
#endif // defined(USE_CAMERA)

namespace orimer{
namespace
{
    ble::BlePacket g_Packet{};

    void PrintInfo(const char* pName, const char* pInfo) noexcept
    {
        Serial.printf("[%s] %s\n", pName, pInfo);
    }

    void PrintError(const char* pName, const char* pError) noexcept
    {
        Serial.printf("[%s][ERROR] %s\n", pName, pError);
    }

    void DetectRobot()
    {
    #if defined(USE_CAMERA)
        camera::Blob blob;

        if (camera::DetectRobot(&blob))
        {
            Serial.printf("Detected at (%d,%d) area=%d\n",
                blob.x, blob.y, blob.area);
        }
        else
        {
            Serial.println("No red");
        }
    #endif // defined(USE_CAMERA)
    }

} // namespace

    void InitializeSystem()
    {
        Serial.begin(115200);
        delay(2000);
        M5.begin(true, true, true);
    }

    void InitializeModules()
    {
#if defined (USE_CAMERA)
        camera::Initialize();
        PrintInfo("Camera", "Initialized");
#endif // defined (USE_CAMERA)

#if defined (ORIMER_HUEDEVICE)
        audio::Initialize();
        fsr::Initialize();
        led::Initialize();
        button::Initialize();
        ble::InitServer();
#elif defined (ORIMER_PIKMIN)
        led::Initialize();
        button::Initialize();
        ble::InitClient();
#endif
        const uint8_t type = static_cast<uint8_t>(ble::GetMode());
        g_Packet = ble::GetEmptyPacket(type);
    }

    void Update()
    {
#if defined (ORIMER_HUEDEVICE)
        fsr::Update();
        fsr::FsrState fsrState{};
        if (fsr::GetState(fsrState))
        {
            Serial.printf(
                "[FSR] raw=%d voltage=%.2f force=%.1f\n",
                fsrState.raw,
                fsrState.voltage,
                fsrState.force
            );
        }

        auto scaledForce = static_cast<uint8_t>(fsrState.force / 2500.f);
        g_Packet.data[0] = scaledForce < 255 ? scaledForce : 255;
        if(fsrState.force > 100)
        {
            uint32_t freq = static_cast<uint32_t>(fsrState.force);
            uint32_t timeMs = 100;
            audio::BeepEx(freq, timeMs);
        }
#endif // defined (ORIMER_HUEDEVICE)

#if defined (USE_CAMERA)
        DetectRobot();
#endif // defined (USE_CAMERA)

        ble::Send(g_Packet);
        ble::Update();

        g_Packet.seq += 1;

        ble::BlePacket recvPacket{};
        if (ble::Receive(recvPacket))
        {
            ble::LogPacket("[Recv]",recvPacket);

            if(recvPacket.data[0] > 200)
            {
                led::SetLed(led::LedColor::Blue);
            }
            else
            {
                led::SetLed(led::LedColor::Green);
            }
        }

        if(!ble::IsConnected())
        {
            orimer::led::SetLed(orimer::led::LedColor::Red);
        }
    }

} // namespace orimer

void setup()
{
    orimer::InitializeSystem();
    orimer::InitializeModules();
}

void loop()
{
    orimer::Update();
    delay(100);
}

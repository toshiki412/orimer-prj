#include <M5Atom.h>
#include "ble/ble_api.h"
#include "ble/ble_types.h"

#include "led/led_api.h"
#include "led/led_types.h"

#include "button/button_api.h"

#include "fsr/fsr_api.h"
#include "fsr/fsr_types.h"

// #define ORIMER_HUEDEVICE
#define ORIMER_PIKMIN

namespace orimer{
namespace
{
    ble::BlePacket g_Packet{};

    void PrintInfo(const char* pName, const char* pInfo) noexcept
    {
        Serial.printf("[%s] %s\n", pName, pInfo);
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
#if defined (ORIMER_HUEDEVICE)
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
        g_Packet.data[0] = static_cast<uint8_t>(fsrState.force / 2500.f);
#endif // defined (ORIMER_HUEDEVICE)

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

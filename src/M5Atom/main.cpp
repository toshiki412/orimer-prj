#include <M5Atom.h>
#include "ble/ble_api.h"
#include "ble/ble_types.h"

#include "led/led_api.h"
#include "led/led_types.h"

#define ORIMER_BLE_SERVER
// #define ORIMER_BLE_CLIENT


namespace orimer{
namespace
{
    ble::ControlState g_State{};

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

    void InitializeLed()
    {
        led::Initialize();
    }

    void InitializeBle()
    {
    #if defined (ORIMER_BLE_SERVER)
        ble::InitServer("Atom-Server");
    #endif // defined (ORIMER_BLE_SERVER)

    #if defined (ORIMER_BLE_CLIENT)
        ble::InitClient();
    #endif // defined (ORIMER_BLE_CLIENT)
    }

    void UpdateBle()
    {
    #if defined (ORIMER_BLE_SERVER)
        g_State.btn++;
        g_State.dir = ble::StickDir::Right;
        ble::Send(g_State);

        if (ble::Receive(g_State))
        {
            led::SetLed(led::LedColor::Green);

            Serial.printf(
                "[MAIN] btn=0x%04X dir=%d\n",
                g_State.btn,
                g_State.dir
            );
        }

        ble::Update();

        delay(100);
    #endif // defined (ORIMER_BLE_SERVER)

    #if defined (ORIMER_BLE_CLIENT)
        ble::Update();

        if (ble::Receive(g_State))
        {
            led::SetLed(led::LedColor::Green);

            Serial.printf(
                "[MAIN] btn=0x%04X dir=%d\n",
                g_State.btn,
                g_State.dir
            );
        }

        delay(50);
    #endif // defined (ORIMER_BLE_CLIENT)
    }

} // namespace orimer

void setup()
{
    orimer::InitializeSystem();
    orimer::InitializeLed();
    orimer::InitializeBle();
}

void loop()
{
    orimer::UpdateBle();
}

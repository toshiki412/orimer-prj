#include <M5Atom.h>
#include "ble/ble_api.h"
#include "ble/ble_types.h"

#include "led/led_api.h"
#include "led/led_types.h"
#include "button/button_api.h"

// #define ORIMER_BLE_SERVER
#define ORIMER_BLE_CLIENT


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

    void InitializeButton()
    {
        button::Initialize();
    }

    void InitializeBle()
    {
    #if defined (ORIMER_BLE_SERVER)
        ble::InitServer();
    #endif // defined (ORIMER_BLE_SERVER)

    #if defined (ORIMER_BLE_CLIENT)
        ble::InitClient();
    #endif // defined (ORIMER_BLE_CLIENT)
    }

    void UpdateBle()
    {
        g_State.btn++;
        g_State.dir = ble::StickDir::Right;
        ble::Send(g_State);
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

        if(!ble::IsConnected())
        {
            orimer::led::SetLed(orimer::led::LedColor::Red);
        }
        delay(100);
    }

} // namespace orimer

void setup()
{
    orimer::InitializeSystem();
    orimer::InitializeLed();
    orimer::InitializeButton();
    orimer::InitializeBle();

}

void loop()
{
    orimer::UpdateBle();
}

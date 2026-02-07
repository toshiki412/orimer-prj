#include <M5Atom.h>
#include "ble/ble_api.h"
#include "ble/ble_types.h"

// #define ORIMER_BLE_SERVER
#define ORIMER_BLE_CLIENT

namespace
{
    orimer::ble::ControlState g_State{};

    void PrintInfo(const char* pName, const char* pInfo) noexcept
    {
        Serial.printf("[%s] %s\n", pName, pInfo);
    }
} // namespace

#if defined (ORIMER_BLE_SERVER)
void setup()
{
    Serial.begin(115200);
    delay(2000);

    M5.begin(true, true, true);
    orimer::ble::InitServer("Atom-Server");

    PrintInfo("Main", "Setup Done");
}

void loop()
{
    g_State.btn++;
    g_State.dir = orimer::ble::StickDir::Right;
    orimer::ble::Send(g_State);
}
#endif // defined (ORIMER_BLE_SERVER)

#if defined (ORIMER_BLE_CLIENT)

void setup()
{
    Serial.begin(115200);
    M5.begin(true, true, true);

    orimer::ble::InitClient();
}

void loop()
{
    orimer::ble::Update();

    if (orimer::ble::Receive(g_State))
    {
        Serial.printf(
            "[MAIN] btn=0x%04X dir=%d\n",
            g_State.btn,
            g_State.dir
        );
    }

    delay(50);
}

#endif // defined (ORIMER_BLE_CLIENT)

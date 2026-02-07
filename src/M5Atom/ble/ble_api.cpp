#include "ble_api.h"

#include <Arduino.h>
#include "ble_server.h"
#include "ble_client.h"

namespace orimer::ble
{
    namespace
    {
        Mode       g_Mode     = Mode::None;
        BleServer* g_pServer  = nullptr;
        BleClient* g_pClient  = nullptr;
    }

void InitServer(const char* pDeviceName)
{
    delete g_pClient;
    g_pClient = nullptr;

    if (g_pServer == nullptr)
    {
        g_pServer = new BleServer();
    }

    g_pServer->Begin(pDeviceName);
    g_Mode = Mode::Server;

    Serial.println("[BLE][API] Init Server");
}

void InitClient(void)
{
    delete g_pServer;
    g_pServer = nullptr;

    if (g_pClient == nullptr)
    {
        g_pClient = new BleClient();
    }

    g_pClient->Begin();
    g_Mode = Mode::Client;

    Serial.println("[BLE][API] Init Client");
}

void Update(void)
{
    if (g_Mode == Mode::Server && g_pServer != nullptr)
    {
        g_pServer->Update();
    }
    
    if (g_Mode == Mode::Client && g_pClient != nullptr)
    {
        g_pClient->Update();
    }
}

bool Send(const ControlState& state)
{
    if (g_Mode != Mode::Server || g_pServer == nullptr)
    {
        return false;
    }

    if (!g_pServer->IsConnected())
    {
        Serial.println("not connected...\n");
        return false;
    }

    g_pServer->SendState(state);
    return true;
}

bool Receive(ControlState& state)
{
    switch (g_Mode)
    {
        case Mode::Server:
            if (!g_pServer->IsConnected())
            {
                return false;
            }
            state = g_pServer->GetState();
            break;
        case Mode::Client:
        {
            if (!g_pClient->IsConnected())
            {
                return false;
            }
            state = g_pClient->GetState();
        }
        default:
            break;
    }

    return true;
}

bool IsConnected(void)
{
    if (g_Mode == Mode::Server && g_pServer != nullptr)
    {
        return g_pServer->IsConnected();
    }

    if (g_Mode == Mode::Client && g_pClient != nullptr)
    {
        return g_pClient->IsConnected();
    }

    return false;
}

Mode GetMode(void)
{
    return g_Mode;
}

} // namespace orimer::ble

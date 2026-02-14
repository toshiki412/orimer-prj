#include "ble_api.h"
#include "ble_config.h"
#include "../config.h"

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

void InitServer()
{
    delete g_pClient;
    g_pClient = nullptr;

    if (g_pServer == nullptr)
    {
        g_pServer = BleServer::GetInstance();
    }

    g_pServer->Begin();
    g_Mode = Mode::Server;

    Serial.println("[BLE][API] Init Server");
}

void InitClient(void)
{
    delete g_pServer;
    g_pServer = nullptr;

    if (g_pClient == nullptr)
    {
        g_pClient = BleClient::GetInstance();
    }

#if defined(USE_COOSPO)
    g_pClient->SetTargetDeviceName(orimer::ble::coospo::ServerName);
    g_pClient->SetTargetServiceUUID(orimer::ble::coospo::ServiceUuid);
    g_pClient->SetTargetCharacteristicUUID(orimer::ble::coospo::CharacteristicUuid);
#else
    g_pClient->SetTargetDeviceName(orimer::ble::atom::ServerName);
    g_pClient->SetTargetServiceUUID(orimer::ble::atom::ServiceUuid);
    g_pClient->SetTargetCharacteristicUUID(orimer::ble::atom::CharacteristicUuid);
#endif

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

bool Send(const BlePacket& packet)
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

    g_pServer->SendPacket(packet);
    return true;
}

bool Receive(BlePacket& packet)
{
    switch (g_Mode)
    {
        case Mode::Server:
            if (!g_pServer->IsConnected())
            {
                return false;
            }
            packet = g_pServer->GetPacket();
            break;
        case Mode::Client:
        {
            if (!g_pClient->IsConnected())
            {
                return false;
            }
            packet = g_pClient->GetPacket();
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

BlePacket GetEmptyPacket(uint8_t type)
{
    BlePacket packet{};
    packet.type = type;
    packet.seq = 0;

    for(int i = 0; i < 14; ++i)
    {
        packet.data[i] = 0;
    }

    return packet;
}

void LogPacket(
    const char* prefix,
    const BlePacket& packet
)
{
    Serial.printf(
        "[BLE][PKT][%s] type=0x%02X seq=%u data=",
        prefix,
        packet.type,
        packet.seq
    );

    for (int i = 0; i < 14; ++i)
    {
        Serial.printf("%02X", packet.data[i]);
        if (i != 13)
        {
            Serial.print(" ");
        }
    }

    Serial.println();
}

} // namespace orimer::ble

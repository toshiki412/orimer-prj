#include "socket_api.h"
#include "socket.h"

namespace orimer::socket
{

namespace
{
    SocketConnectionTool g_Socket;
    bool g_IsConnected = false;
}

bool Initialize()
{
    g_Socket.Initialize();
    g_IsConnected = true;
    return true;
}

void Finalize()
{
    g_Socket.Finalize();
    g_IsConnected = false;
}

bool Send(uint64_t timeStamp, int32_t* data, int dataSize)
{
    if (!g_IsConnected)
        return false;

    g_Socket.SetSendData(timeStamp, data, dataSize);
    return true;
}

bool Receive(uint64_t* pOutTimeStamp, int32_t* pOutData, int dataSize)
{
    if (!g_IsConnected)
        return false;

    g_Socket.GetRecvData(pOutTimeStamp, pOutData, dataSize);
    return true;
}

bool IsConnected()
{
    return g_IsConnected;
}

}

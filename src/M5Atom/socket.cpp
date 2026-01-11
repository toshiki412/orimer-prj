/******************************************************************************
 * Copyright (c) 2025 [orimer]. All rights reserved.
 *
 * Unauthorized reproduction or distribution of this source code is prohibited.
 * No part of this code may be copied, reproduced, or distributed without
 * the prior written permission of the copyright holder.
 ******************************************************************************/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "socket.h"


namespace {

    constexpr const char* SERVER_IP   = "192.168.40.72";
    constexpr int         SERVER_PORT = 5000;
    constexpr int         ERROR_CODE  = 2;

    int CreateSocket()
    {
        // ソケット作成
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "[Error] Socket creation failed\n";
            return -1;
        }
        return sock;
    }

    void ConnectServer(int sock)
    {
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERVER_PORT);
        if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
            std::cerr << "[Error] Invalid address\n";
            std::exit(ERROR_CODE);
        }

        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "[Error] Connection Failed\n";
            std::exit(ERROR_CODE);
        }

        std::cout << "[Success] Connect Server !!" << std::endl;
    }
}// namespace


void SocketConnectionTool::Initialize()
{
    m_socket = CreateSocket();
    ConnectServer(m_socket);
}

void SocketConnectionTool::Finalize()
{
    close(m_socket);
}

void SocketConnectionTool::SetSendData(uint64_t timeStamp, int32_t data[], int dataSize)
{
    constexpr int BYTE_2_BIT = 8;

    // 送信データのパック
    const int sizeOfSendBuffer = dataSize * sizeof(int32_t) + sizeof(uint64_t);
    if(sizeOfSendBuffer > SendBufferSize)
    {
        std::cerr << "[Error] Exceeded maximum send buffer size\n";
        return;
    }

    // data[]
    for (int j = 0; j < dataSize; ++j)
    {
        for (int i = 0; i < static_cast<int>(sizeof(int32_t)); ++i)
        {
            m_sendBuffer[j * sizeof(int32_t) + i] =
                (data[j] >> (BYTE_2_BIT * (sizeof(int32_t) - 1 - i))) & 0xFF;
        }
    }

    // タイムスタンプ
    const int tsOffset = dataSize * sizeof(int32_t);
    for (int i = 0; i < static_cast<int>(sizeof(uint64_t)); ++i)
    {
        m_sendBuffer[tsOffset + i] =
            (timeStamp >> (BYTE_2_BIT * (sizeof(uint64_t) - 1 - i))) & 0xFF;
    }

    // 送信
    send(m_socket, m_sendBuffer, sizeOfSendBuffer, 0);
}

void SocketConnectionTool::GetRecvData(uint64_t* pOutTimeStamp, int32_t pOutdata[], int dataSize)
{
    constexpr int BYTE_2_BIT = 8;
    const int sizeOfRecvBuffer = dataSize * sizeof(int32_t) + sizeof(uint64_t);
    if(sizeOfRecvBuffer > RecvBufferSize)
    {
        std::cerr << "[Error] Exceeded maximum receive buffer size\n";
        return;
    }

    int received = 0;
    while (received < sizeOfRecvBuffer) {
        int n = recv(m_socket, m_recvBuffer + received, sizeOfRecvBuffer - received, 0);
        if (n <= 0) 
        {
            return;
        }
        received += n;
    }

    for(int j = 0; j < dataSize; ++j) {
        for(int i = 0; i < sizeof(int32_t); ++i) {
            int shift = (sizeof(int32_t) - 1 - i) * BYTE_2_BIT;
            pOutdata[j] |= (static_cast<int32_t>(m_recvBuffer[j * sizeof(int32_t) + i]) << shift);
        }
    }

    // タイムスタンプ
    uint64_t t = 0;
    const int tsOffset = dataSize * sizeof(int32_t);
    for(int i = 0; i < sizeof(uint64_t); ++i) {
        int shift = (sizeof(uint64_t) - 1 - i) * BYTE_2_BIT;
        t |= (static_cast<uint64_t>(m_recvBuffer[tsOffset + i]) << shift);
    }
    *pOutTimeStamp = t;
}
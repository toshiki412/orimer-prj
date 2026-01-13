 /******************************************************************************
 * Copyright (c) 2025 [orimer]. All rights reserved.
 *
 * Unauthorized reproduction or distribution of this source code is prohibited.
 * No part of this code may be copied, reproduced, or distributed without
 * the prior written permission of the copyright holder.
 ******************************************************************************/

#include <sys/socket.h>
#include <arpa/inet.h>


class SocketConnectionTool
{
    public:
        void Initialize();

        void SetSendData(uint64_t timeStamp, int32_t data[], int dataSize);
        void GetRecvData(uint64_t* pOutTimeStamp, int32_t data[], int dataSize);

        void Finalize();

    private:
        constexpr static int MaxDataSize = 10;
        constexpr static int SendBufferSize = MaxDataSize * sizeof(int32_t) + sizeof(uint64_t);
        constexpr static int RecvBufferSize = MaxDataSize * sizeof(int32_t) + sizeof(uint64_t);
        uint8_t m_sendBuffer[SendBufferSize];
        uint8_t m_recvBuffer[RecvBufferSize];
        int m_socket;
};

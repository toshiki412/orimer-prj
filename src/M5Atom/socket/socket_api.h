#pragma once
#include <cstdint>

namespace orimer::socket
{

// 初期化（接続）
bool Initialize();

// 切断
void Finalize();

// 送信
bool Send(uint64_t timeStamp, int32_t* data, int dataSize);

// 受信
bool Receive(uint64_t* pOutTimeStamp, int32_t* pOutData, int dataSize);

// 接続状態確認
bool IsConnected();

}

#include "coospo_api.h"

namespace orimer::coospo
{
    namespace
    {
        uint8_t g_HeartRate = 0;
    }

    void Update(
        const uint8_t* pData,
        size_t length)
    {
        if (length < 2) return  ; // データ長が不十分な場合は処理しない

        uint8_t flags = pData[0];

        if (flags & 0x01)
        {
            g_HeartRate = pData[1] | (pData[2] << 8);
        }
        else
        {
            g_HeartRate = pData[1];
        }
    }

    uint8_t GetHeartRate()
    {
        return g_HeartRate;
    }
}
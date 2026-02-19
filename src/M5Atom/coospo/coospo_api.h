#pragma once
#include <stdint.h>
#include <cstddef>   // size_t

namespace orimer::coospo
{
    void Update(
        const uint8_t* pData,
        size_t length);

    uint8_t GetHeartRate();
}

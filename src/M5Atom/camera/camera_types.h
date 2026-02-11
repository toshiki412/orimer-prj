#pragma once
#include <stdint.h>

namespace orimer::camera
{

struct Blob
{
    int16_t x;
    int16_t y;
    uint16_t area;
    bool detected;
};

}

#pragma once

#include <cstdint>

namespace orimer::audio {

enum class BeepTone : uint8_t
{
    Short,
    Long,
    Error,
};

} // namespace orimer::audio

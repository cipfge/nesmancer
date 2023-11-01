#pragma once

#include <cstdint>

union Word
{
    uint16_t value = 0;
    struct
    {
        uint8_t byte_low;
        uint8_t byte_high;
    };
};

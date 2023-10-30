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

class Memory
{
public:
    Memory();
    ~Memory();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    uint8_t m_internal_ram[2048];
};

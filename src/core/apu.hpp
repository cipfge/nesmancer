#pragma once

#include <cstdint>

class APU
{
public:
    APU();
    ~APU();

    void reset();
    void tick();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};

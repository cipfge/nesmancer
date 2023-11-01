#pragma once

#include "types.hpp"

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

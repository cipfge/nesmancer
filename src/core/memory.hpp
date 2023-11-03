#pragma once

#include "types.hpp"

class PPU;
class Cartridge;

class Memory
{
public:
    Memory(PPU* ppu, Cartridge* cartridge);
    ~Memory();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    uint8_t m_internal_ram[2048];
    PPU* m_ppu = nullptr;
    Cartridge* m_cartrige = nullptr;
};

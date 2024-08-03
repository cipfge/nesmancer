#include "mapper_uxrom.hpp"

Mapper_UxROM::Mapper_UxROM(NesRom& rom):
    Mapper(rom)
{
    configure();
}

void Mapper_UxROM::cpu_write(uint16_t address, uint8_t data)
{
    if (address & 0x8000)
    {
        m_register = data;
        configure();
    }
}

void Mapper_UxROM::ppu_write(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}

void Mapper_UxROM::configure()
{
    map_prg<16>(0, m_register & 0xF);
    map_prg<16>(1, 0xF);

    map_chr<8>(0, 0);
}

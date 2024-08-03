#include "mapper_cnrom.hpp"

Mapper_CNROM::Mapper_CNROM(NesRom& rom) :
    Mapper(rom)
{
    configure();
}

void Mapper_CNROM::cpu_write(uint16_t address, uint8_t data)
{
    if (address & 0x8000)
    {
        m_register = data;
        configure();
    }
}

void Mapper_CNROM::ppu_write(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}

void Mapper_CNROM::configure()
{
    if (m_prg_banks == 1)
    {
        map_prg<16>(0, 0);
        map_prg<16>(1, 0);
    }
    else
    {
        map_prg<16>(0, 0);
        map_prg<16>(1, 1);
    }

    map_chr<8>(0, m_register & 0b11);
}

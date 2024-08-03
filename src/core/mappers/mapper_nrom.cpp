#include "mapper_nrom.hpp"
#include "global.hpp"

Mapper_NROM::Mapper_NROM(NesRom& rom) :
    Mapper(rom)
{
    map_prg<32>(0, 0);
    map_chr<8>(0, 0);
}

void Mapper_NROM::cpu_write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(address);
    EMU_UNUSED(data);
}

void Mapper_NROM::ppu_write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(address);
    EMU_UNUSED(data);
}

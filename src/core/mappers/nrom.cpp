#include "nrom.hpp"
#include "global.hpp"

NROM::NROM(uint8_t prg_banks,
           uint8_t chr_banks,
           MirroringMode mirroring_mode)
    : Mapper(0, prg_banks, chr_banks, mirroring_mode)
{
}

NROM::~NROM()
{
}

uint32_t NROM::read(uint16_t address)
{
    return map_address(address);
}

uint32_t NROM::write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(data);
    return map_address(address);
}

uint32_t NROM::map_address(uint16_t address)
{
    if (address < 0x2000)
        return address;
    else if (address < 0x3F00)
    {
        uint32_t mapped_address = address & 0x0FFF;
        if (m_mirroring_mode == MIRROR_VERTICAL)
            return mapped_address & 0x07FF;
        else
        {
            if (mapped_address < 0x0800)
                return mapped_address & 0x03FF;
            else
                return ((mapped_address - 0x0800) & 0x03FF) + 0x0400;
        }
    }
    else if (address < 0x6000)
        return address;
    else if (address < 0x8000)
        return address - 0x6000;
    else if (address < 0xC000)
        return address - 0x8000;
    else
        return m_prg_banks == 1 ? address - 0xC000 : address - 0x8000;
}
#include "mapper.hpp"
#include "global.hpp"

Mapper::Mapper(uint16_t id, uint8_t prg_banks,
               uint8_t chr_banks, MirroringMode mirroring_mode)
    : m_id(id)
    , m_prg_banks(prg_banks)
    , m_chr_banks(chr_banks)
    , m_mirroring_mode(mirroring_mode)
{
}

Mapper::~Mapper()
{
}

Mapper0::Mapper0(uint8_t prg_banks, uint8_t chr_banks, MirroringMode mirroring_mode)
    : Mapper(0, prg_banks, chr_banks, mirroring_mode)
{
}

Mapper0::~Mapper0()
{
}

uint32_t Mapper0::read(uint16_t address)
{
    return map_address(address);
}

uint32_t Mapper0::write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(data);
    return map_address(address);
}

uint32_t Mapper0::map_address(uint16_t address)
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

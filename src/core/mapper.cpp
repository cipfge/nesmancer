#include "mapper.hpp"
#include "global.hpp"

Mapper::Mapper(uint16_t id, uint8_t prg_banks,
               uint8_t chr_banks, MirrorMode mirror_mode)
    : m_id(id)
    , m_prg_banks(prg_banks)
    , m_chr_banks(chr_banks)
    , m_mirror_mode(mirror_mode)
{
}

Mapper::~Mapper()
{
}

Mapper0::Mapper0(uint8_t prg_banks, uint8_t chr_banks, MirrorMode mirror_mode)
    : Mapper(0, prg_banks, chr_banks, mirror_mode)
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
    if (address >= 0x2000 && address < 0x3F00)
    {
        uint32_t mapped_address = address & 0x0FFF;
        if (m_mirror_mode == MIRROR_VERTICAL)
            return mapped_address & 0x07FF;
        else
            return (mapped_address < 0x0800) ?
                   (mapped_address & 0x03FF) :
                   ((mapped_address - 0x0800) & 0x03FF) + 0x0400;
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

bool Mapper0::cpu_irq()
{
    return false;
}

void Mapper0::cpu_irq_clear()
{
}

void Mapper0::ppu_scanline()
{
}

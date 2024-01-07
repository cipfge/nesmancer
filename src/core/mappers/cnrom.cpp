#include "cnrom.hpp"

CNROM::CNROM(uint8_t prg_bank_count,
             uint8_t chr_bank_count,
             MirroringMode mirroring_mode)
    : Mapper(MAPPER_CNROM, prg_bank_count, chr_bank_count, mirroring_mode)
{
}

CNROM::~CNROM()
{
}

uint32_t CNROM::read(uint16_t address)
{
    return map_address(address);
}

uint32_t CNROM::write(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
        m_chr_bank = data;

    return map_address(address);
}

uint32_t CNROM::map_address(uint16_t address) const
{
    if (address < 0x2000)
        return address + ((m_chr_bank % m_chr_bank_count) * Size_8KB);
    else if (address < 0x3F00)
    {
        uint32_t mapped_address = address & 0x0FFF;
        if (m_mirroring_mode == MirroringMode::Vertical)
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
        return (address - 0x8000);
    else
    {
        if (m_prg_bank_count == 1)
            return address - 0xC000;
        else
            return address - 0x8000;
    }
}

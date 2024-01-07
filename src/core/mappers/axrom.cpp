#include "axrom.hpp"

AxROM::AxROM(uint8_t prg_bank_count,
             uint8_t chr_bank_count,
             MirroringMode mirroring_mode)
    : Mapper(MAPPER_AXROM, prg_bank_count, chr_bank_count, mirroring_mode)
{
}

AxROM::~AxROM()
{
}

uint32_t AxROM::read(uint16_t address)
{
    return map_address(address);
}

uint32_t AxROM::write(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
        m_register = data;
    return map_address(address);
}

uint32_t AxROM::map_address(uint16_t address) const
{
    if (address < 0x2000)
        return address;
    else if (address < 0x3F00)
    {
        uint32_t mapped_address = address & 0x0FFF;
        if ((m_register >> 4) & 0x1)
            return (mapped_address & 0x03FF) + 0x0400;
        else
            return mapped_address & 0x03FF;
    }
    else if (address < 0x6000)
        return address;
    else if (address < 0x8000)
        return address - 0x6000;
    else
    {
        uint8_t bank = (m_register & 0xF) % (m_prg_bank_count / 2);
        return (address - 0x8000) + (bank * Size_32KB);
    }
}

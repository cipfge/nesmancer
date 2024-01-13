#include "uxrom.hpp"

uint32_t UxROM::read(uint16_t address)
{
    return map_address(address);
}

uint32_t UxROM::write(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
        m_program_bank = data;

    return map_address(address);
}

uint32_t UxROM::map_address(uint16_t address) const
{
    if (address < 0x2000)
        return address;
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
        return (address - 0x8000) + (m_program_bank * 0x4000);
    else
        return (address - 0xC000) + ((m_program_bank_count - 1) * 0x4000);
}

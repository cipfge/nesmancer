#include "mmc1.hpp"
#include "logger.hpp"

uint32_t MMC1::read(uint16_t address)
{
    return map_address(address);
}

uint32_t MMC1::write(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
    {
        if ((data >> 7) & 0x1)
        {
            m_shift_register = 0;
            m_shift_count = 0;
            m_control_register |= 0xC;
        }
        else
        {
            m_shift_register = (m_shift_register & 0x1F) | ((data & 0x1) << 5);
            m_shift_register >>= 1;
            m_shift_count++;

            if (m_shift_count == 5)
            {
                if (address < 0xA000)
                    m_control_register = m_shift_register;
                else if (address < 0xC000)
                    m_character_bank0 = m_shift_register;
                else if (address < 0xE000)
                    m_character_bank1 = m_shift_register;
                else
                    m_program_bank = m_shift_register;

                m_shift_register = 0;
                m_shift_count = 0;
            }
        }
    }

    return map_address(address);
}

uint32_t MMC1::map_address(uint16_t address) const
{
    if (address < 0x2000)
    {
        if ((m_control_register >> 4) & 0x1)
        {
            if (address < 0x1000)
                return address + m_character_bank0 * 0x1000;
            else
                return (address - 0x1000) + m_character_bank1 * 0x1000;
        }
        else
        {
            return address + m_character_bank0 * 0x2000;
        }
    }
    else if (address < 0x3F00)
    {
        uint32_t mapped_address = address & 0x0FFF;
        uint8_t mirror_mode = m_control_register & 0x3;

        switch (mirror_mode)
        {
        case NT_ONE_SCREEN_LO: return mapped_address & 0x03FF;
        case NT_ONE_SCREEN_HI: return (mapped_address & 0x03FF) + 0x0400;
        case NT_VERTICAL: return mapped_address & 0x07FF;
        case NT_HORIZONTAL:
        {
            if (mapped_address < 0x0800)
                return mapped_address & 0x03FF;
            else
                return ((mapped_address - 0x0800) & 0x03FF) + 0x0400;
        }
        default:
            LOG_ERROR("Invalid nametable mirror mode %u", mirror_mode);
            return 0;
        }
    }
    else if (address < 0x6000)
    {
        return address;
    }
    else if (address < 0x8000)
    {
        return address - 0x6000;
    }
    else if (address < 0xC000)
    {
        uint8_t bank_mode = (m_control_register >> 2) & 0x3;
        switch (bank_mode)
        {
        case 0:
        case 1: return (address - 0x8000) + ((m_program_bank & 0x0F) * 0x8000);
        case 2: return address - 0x8000;
        case 3: return (address - 0x8000) + ((m_program_bank & 0x0F) * 0x4000);
        default:
            LOG_ERROR("Invalid PRG bank mode %u", bank_mode);
            return 0;
        }
    }
    else
    {
        uint8_t bank_mode = (m_control_register >> 2) & 0x3;
        switch (bank_mode)
        {
        case 0:
        case 1: return (address - 0x8000) + ((m_program_bank & 0x0F) * 0x8000);
        case 2: return (address - 0xC000) + ((m_program_bank & 0x0F) * 0x4000);
        case 3: return (address - 0xC000) + ((m_program_bank_count - 1) * 0x4000);
        default:
            LOG_ERROR("Invalid PRG bank mode %u", bank_mode);
            return 0;
        }
    }
}

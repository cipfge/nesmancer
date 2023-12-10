#include "mmc3.hpp"
#include <cstring>

MMC3::MMC3(uint8_t prg_bank_count,
           uint8_t chr_bank_count,
           MirroringMode mirroring_mode)
    : Mapper(MAPPER_MMC3, prg_bank_count, chr_bank_count, mirroring_mode)
{
    memset(m_bank_registers, 0x00, sizeof(m_bank_registers));
    memset(m_chr_bank, 0x00, sizeof(m_chr_bank));

    m_prg_bank[0] = 0;
    m_prg_bank[1] = Size_8KB;
    m_prg_bank[2] = (prg_bank_count * 2 - 2) * Size_8KB;
    m_prg_bank[3] = (prg_bank_count * 2 - 1) * Size_8KB;
}

MMC3::~MMC3()
{
}

uint32_t MMC3::read(uint16_t address)
{
    return map_address(address);
}

uint32_t MMC3::write(uint16_t address, uint8_t data)
{
    bool odd = address & 0x1;

    if (address < 0x8000)
    {
    }
    else if (address < 0xA000)
    {
        if (odd)
        {
            m_bank_registers[m_bank_target] = data;

            if (m_chr_inversion)
            {
                m_chr_bank[0] = m_bank_registers[2] * Size_1KB;
                m_chr_bank[1] = m_bank_registers[3] * Size_1KB;
                m_chr_bank[2] = m_bank_registers[4] * Size_1KB;
                m_chr_bank[3] = m_bank_registers[5] * Size_1KB;
                m_chr_bank[4] = (m_bank_registers[0] & 0xFE) * Size_1KB;
                m_chr_bank[5] = (m_bank_registers[0] + 1) * Size_1KB;
                m_chr_bank[6] = (m_bank_registers[1] & 0xFE) * Size_1KB;
                m_chr_bank[7] = (m_bank_registers[1] + 1) * Size_1KB;
            }
            else
            {
                m_chr_bank[0] = (m_bank_registers[0] & 0xFE) * Size_1KB;
                m_chr_bank[1] = (m_bank_registers[0] + 1) * Size_1KB;
                m_chr_bank[2] = (m_bank_registers[1] & 0xFE) * Size_1KB;
                m_chr_bank[3] = (m_bank_registers[1] + 1) * Size_1KB;
                m_chr_bank[4] = m_bank_registers[2] * Size_1KB;
                m_chr_bank[5] = m_bank_registers[3] * Size_1KB;
                m_chr_bank[6] = m_bank_registers[4] * Size_1KB;
                m_chr_bank[7] = m_bank_registers[5] * Size_1KB;
            }

            if (m_prg_bank_mode)
            {
                m_prg_bank[0] = (m_prg_bank_count * 2 - 2) * Size_8KB;
                m_prg_bank[2] = (m_bank_registers[6] & 0x3F) * Size_8KB;
            }
            else
            {
                m_prg_bank[0] = (m_bank_registers[6] & 0x3F) * Size_8KB;
                m_prg_bank[2] = (m_prg_bank_count * 2 - 2) * Size_8KB;
            }

            m_prg_bank[1] = (m_bank_registers[7] & 0x3F) * Size_8KB;
            m_prg_bank[3] = (m_prg_bank_count * 2 - 1) * Size_8KB;
        }
        else
        {
            m_bank_target = data & 0x7;
            m_prg_bank_mode = (data >> 6) & 0x1;
            m_chr_inversion = (data >> 7) & 0x1;
        }
    }
    else if (address < 0xC000)
    {
        if (odd)
        {
            // TODO: Write protect and PRG RAM enable
        }
        else
        {
            if (data & 1)
                m_mirroring_mode = MirroringMode::Horizontal;
            else
                m_mirroring_mode = MirroringMode::Vertical;
        }
    }
    else if (address < 0xE000)
    {
        if (odd)
            m_irq_counter = 0;
        else
            m_irq_reload = data;
    }
    else
    {
        if (odd)
            m_irq_enabled = true;
        else
        {
            m_irq_enabled = false;
            m_irq = false;
        }
    }

    return map_address(address);
}

void MMC3::scanline()
{
    if (m_irq_counter > 0)
        m_irq_counter--;
    else
        m_irq_counter = m_irq_reload;

    if (m_irq_counter == 0 && m_irq_enabled)
        m_irq = true;
}

uint32_t MMC3::map_address(uint16_t address) const
{
    if (address < 0x0400)
        return m_chr_bank[0] + (address & 0x03FF);
    else if (address < 0x0800)
        return m_chr_bank[1] + (address & 0x03FF);
    else if (address < 0x0C00)
        return m_chr_bank[2] + (address & 0x03FF);
    else if (address < 0x1000)
        return m_chr_bank[3] + (address & 0x03FF);
    else if (address < 0x1400)
        return m_chr_bank[4] + (address & 0x03FF);
    else if (address < 0x1800)
        return m_chr_bank[5] + (address & 0x03FF);
    else if (address < 0x1C00)
        return m_chr_bank[6] + (address & 0x03FF);
    else if (address < 0x2000)
        return m_chr_bank[7] + (address & 0x03FF);
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
        return address & 0x1FFF;
    else if (address < 0xA000)
        return m_prg_bank[0] + (address & 0x1FFF);
    else if (address < 0xC000)
        return m_prg_bank[1] + (address & 0x1FFF);
    else if (address < 0xE000)
        return m_prg_bank[2] + (address & 0x1FFF);
    else
        return m_prg_bank[3] + (address & 0x1FFF);
}

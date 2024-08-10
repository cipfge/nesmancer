#include "mapper_mmc1.hpp"

Mapper_MMC1::Mapper_MMC1(NesRom& rom) :
    Mapper(rom)
{
    m_registers[0] = 0x0C;
    m_registers[1] = 0x00;
    m_registers[2] = 0x00;
    m_registers[3] = 0x00;

    configure();
}

void Mapper_MMC1::cpu_write(uint16_t address, uint8_t data)
{
    if (address < 0x8000)
    {
        m_prg_ram[address - 0x6000] = data;
    }
    else if (address & 0x8000)
    {
        if (data & 0x80)
        {
            m_count = 0;
            m_shift_register = 0;
            m_registers[0] |= 0x0C;

            configure();
        }
        else
        {
            m_shift_register = ((data & 1) << 4) | (m_shift_register >> 1);
            if (++m_count == 5)
            {
                m_registers[(address >> 13) & 0b11] = m_shift_register;
                m_count = 0;
                m_shift_register = 0;

                configure();
            }
        }
    }
}

void Mapper_MMC1::ppu_write(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}

void Mapper_MMC1::configure()
{
    if (m_registers[0] & 0b1000)
    {
        if (m_registers[0] & 0b100)
        {
            map_prg(16, 0, m_registers[3] & 0xF);
            map_prg(16, 1, 0xF);
        }
        else
        {
            map_prg(16, 0, 0);
            map_prg(16, 1, m_registers[3] & 0xF);
        }
    }
    else
    {
        map_prg(32, 0, (m_registers[3] & 0xF) >> 1);
    }

    if (m_registers[0] & 0b10000)
    {
        map_chr(4, 0, m_registers[1]);
        map_chr(4, 1, m_registers[2]);
    }
    else
    {
        map_chr(8, 0, m_registers[1] >> 1);
    }

    switch (m_registers[0] & 0b11)
    {
    case 2:
        m_mirroring_mode = MirroringMode::Vertical;
        break;

    case 3:
        m_mirroring_mode = MirroringMode::Horizontal;
        break;

    default:
        break;
    }
}

#include "mapper_mmc3.hpp"

Mapper_MMC3::Mapper_MMC3(NesRom& rom) :
    Mapper(rom)
{
    map_prg(8, 3, -1);
    configure();
}

void Mapper_MMC3::cpu_write(uint16_t address, uint8_t data)
{
    if (address < 0x8000)
        m_prg_ram[address - 0x6000] = data;
    else if (address & 0x8000)
    {
        switch (address & 0xE001)
        {
        case 0x8000: m_tregister = data; break;
        case 0x8001: m_registers[m_tregister & 0b111] = data; break;
        case 0xA000: m_horizontal_mirroring = data & 1; break;
        case 0xC000: m_irq_time = data; break;
        case 0xC001: m_irq_count = 0; break;
        case 0xE000: m_irq = (!m_irq_enabled); break;
        case 0xE001: m_irq_enabled = true; break;
        }

        configure();
    }
}

void Mapper_MMC3::ppu_write(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}

void Mapper_MMC3::scanline()
{
    if (m_irq_count == 0)
        m_irq_count = m_irq_time;
    else
        m_irq_count--;

    if (m_irq_enabled and m_irq_count == 0)
        m_irq = true;
}

void Mapper_MMC3::configure()
{
    map_prg(8, 1, m_registers[7]);

    if (!(m_tregister & (1 << 6)))
    {
        map_prg(8, 0, m_registers[6]);
        map_prg(8, 2, -2);
    }
    else
    {
        map_prg(8, 0, -2);
        map_prg(8, 2, m_registers[6]);
    }

    if (!(m_tregister & (1 << 7)))
    {
        map_chr(2, 0, m_registers[0] >> 1);
        map_chr(2, 1, m_registers[1] >> 1);

        for (int i = 0; i < 4; i++)
            map_chr(1, 4 + i, m_registers[2 + i]);
    }
    else
    {
        for (int i = 0; i < 4; i++)
            map_chr(1, i, m_registers[2 + i]);

        map_chr(2, 2, m_registers[0] >> 1);
        map_chr(2, 3, m_registers[1] >> 1);
    }

    m_mirroring_mode = m_horizontal_mirroring ? MirroringMode::Horizontal : MirroringMode::Vertical;
}

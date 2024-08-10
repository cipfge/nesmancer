#include "mapper.hpp"

Mapper::Mapper(NesRom& rom)
{
    m_id = rom.mapper_id();
    m_prg_banks = rom.program_banks();
    m_prg_size = rom.program_rom_size();
    m_prg_ram_size = rom.program_ram_size();
    m_chr_banks = rom.character_banks();
    m_chr_size = rom.character_rom_size();
    m_mirroring_mode = rom.mirroring_mode();

    m_prg_ram.resize(m_prg_ram_size);

    m_prg.resize(m_prg_size);
    rom.read_prg_data(m_prg);
 
    if (m_chr_size == 0)
    {
        m_chr_size = 0x2000;
        m_chr.resize(m_chr_size);
    }
    else
    {
        m_chr.resize(m_chr_size);
        rom.read_chr_data(m_chr);
    }
}

uint8_t Mapper::cpu_read(uint16_t address)
{
    if (address < 0x6000)
        return 0x00; // Expansion ROM, not supported
    else if (address < 0x8000)
        return m_prg_ram[address - 0x6000];
    else
        return m_prg[m_prg_mapping[(address - 0x8000) / 0x2000] + ((address - 0x8000) % 0x2000)];
}

uint8_t Mapper::ppu_read(uint16_t address)
{
    return m_chr[m_chr_mapping[address / 0x400] + (address % 0x400)];
}

void Mapper::map_prg(uint32_t size_kb, uint16_t slot, uint16_t bank)
{
    if (bank < 0)
        bank = (m_prg_size / (0x400 * size_kb)) + bank;

    for (int i = 0; i < (size_kb / 8); i++)
        m_prg_mapping[(size_kb / 8) * slot + i] = (size_kb * 0x400 * bank + 0x2000 * i) % m_prg_size;
}

void Mapper::map_chr(uint32_t size_kb, uint16_t slot, uint16_t bank)
{
    for (int i = 0; i < size_kb; i++)
        m_chr_mapping[size_kb * slot + i] = (size_kb * 0x400 * bank + 0x400 * i) % m_chr_size;
}

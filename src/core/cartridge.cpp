#include "cartridge.hpp"
#include "nrom.hpp"
#include "logger.hpp"
#include <fstream>
#include <cstring>

Cartridge::~Cartridge()
{
}

void Cartridge::reset()
{
    m_prg_ram.clear();
    m_prg_rom.clear();
    m_chr_rom.clear();
    memset(m_vram, 0, sizeof(m_vram));

    m_mapper_id = 0;
    m_prg_banks = 0;
    m_chr_banks = 0;
    m_prg_ram_size = 0;
    m_prg_rom_size = 0;
    m_chr_rom_size = 0;

    m_mapper.reset();

    m_has_trainer = false;
    m_use_chr_ram = false;
    m_loaded = false;
}

bool Cartridge::load_from_file(const std::string& file_path)
{
    std::ifstream rom_file(file_path, std::ifstream::binary);
    if (!rom_file.is_open())
    {
        LOG_ERROR("Failed to open rom file %s", file_path.c_str());
        return false;
    }

    uint8_t nes_header[16] = {0};
    if (!rom_file.read(reinterpret_cast<char*>(&nes_header), sizeof(nes_header)))
        return false;

    if (strncmp((char *)nes_header, "NES\x1A", 4) != 0)
    {
        LOG_ERROR("Invalid signature for file %s", file_path.c_str());
        return false;
    }

    reset();
    parse_nes_header(nes_header);

    switch (m_mapper_id)
    {
    case 0:
        m_mapper = std::make_shared<NROM>(m_prg_banks,
                                          m_chr_banks,
                                          m_mirroring_mode);
        break;

    default:
        LOG_ERROR("Unsupported mapper id %u", m_mapper_id);
        return false;
    }

    m_prg_ram.resize(m_prg_ram_size);

    // Skip trainer information
    if (m_has_trainer)
        rom_file.seekg(512, std::ios::cur);

    m_prg_rom.resize(m_prg_rom_size);
    if (!rom_file.read(reinterpret_cast<char*>(m_prg_rom.data()), m_prg_rom.size()))
    {
        LOG_ERROR("Error while reading PRG data from %s", file_path.c_str());
        return false;
    }

    if (m_chr_rom_size == 0)
    {
        m_use_chr_ram = true;
        m_chr_rom.resize(m_prg_ram_size);
    }
    else
    {
        m_chr_rom.resize(m_chr_rom_size);
        if (!rom_file.read(reinterpret_cast<char*>(m_chr_rom.data()), m_chr_rom.size()))
        {
            LOG_ERROR("Error while reading CHR data from %s", file_path.c_str());
            return false;
        }
    }

    m_loaded = true;
    return true;
}

uint8_t Cartridge::cpu_read(uint16_t address)
{
    uint32_t mapped_address = m_mapper->read(address);

    if (address >= 0x6000 && address < 0x8000)
        return m_prg_ram[mapped_address];
    else
        return m_prg_rom[mapped_address];

    return 0;
}

void Cartridge::cpu_write(uint16_t address, uint8_t data)
{
    uint32_t mapped_address = m_mapper->write(address, data);

    if (address >= 0x6000 && address < 0x8000)
        m_prg_ram[mapped_address] = data;
}

uint8_t Cartridge::ppu_read(uint16_t address)
{
    uint32_t mapped_address = m_mapper->read(address);

    if (address < 0x2000)
        return m_chr_rom[mapped_address];
    return m_vram[mapped_address];
}

void Cartridge::ppu_write(uint16_t address, uint8_t data)
{
    uint32_t mapped_address = m_mapper->write(address, data);

    if (address < 0x2000)
        m_chr_rom[mapped_address] = data;
    else
        m_vram[mapped_address] = data;
}

bool Cartridge::irq()
{
    if (!m_loaded)
        return false;
    return m_mapper->irq();
}

void Cartridge::irq_clear()
{
    if (!m_loaded)
        return;
    m_mapper->irq_clear();
}

void Cartridge::scanline()
{
    if (!m_loaded)
        return;
    m_mapper->scanline();
}

void Cartridge::parse_nes_header(const uint8_t* header)
{
    m_mapper_id = (header[6] >> 4) | (header[7] & 0xF0);
    m_prg_banks = header[4];
    m_chr_banks = header[5];
    m_prg_ram_size = header[8] > 0 ? header[8] * 8192 : 8192;
    m_prg_rom_size = static_cast<uint32_t>(header[4] << 14);
    m_chr_rom_size = static_cast<uint32_t>(header[5] << 13);
    m_mirroring_mode = header[6] & 0x1 ? MIRROR_VERTICAL: MIRROR_HORIZONTAL;
    m_has_trainer = header[6] & 0x4;
}

#include "cartridge.hpp"
#include "nrom.hpp"
#include "mmc1.hpp"
#include "uxrom.hpp"
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

    memset(m_vram, 0xFF, sizeof(m_vram));

    m_mapper.reset();
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

    uint16_t mapper_id = (nes_header[6] >> 4) | (nes_header[7] & 0xF0);
    uint8_t prg_bank_count = nes_header[4];
    uint8_t chr_bank_count = nes_header[5];
    uint16_t prg_ram_size = (nes_header[8] > 0 ? nes_header[8] * Mapper::SIZE_8KB : Mapper::SIZE_8KB);
    uint16_t chr_rom_size = chr_bank_count * Mapper::SIZE_8KB;
    MirroringMode mirroring_mode = nes_header[6] & 0x1 ? MIRROR_VERTICAL : MIRROR_HORIZONTAL;
    bool trainer = nes_header[6] & 0x4;

    switch (mapper_id)
    {
    case 0:
        m_mapper = std::make_shared<NROM>(prg_bank_count, chr_bank_count, mirroring_mode);
        break;

    case 1:
        m_mapper = std::make_shared<MMC1>(prg_bank_count, chr_bank_count, mirroring_mode);
        break;

    case 2:
        m_mapper = std::make_shared<UxROM>(prg_bank_count, chr_bank_count, mirroring_mode);
        break;

    default:
        LOG_ERROR("Unsupported mapper id %u", mapper_id);
        return false;
    }

    // Skip trainer information
    if (trainer)
        rom_file.seekg(512, std::ios::cur);

    m_prg_ram.resize(prg_ram_size);
    m_prg_rom.resize(prg_bank_count * Mapper::SIZE_16KB);
    if (!rom_file.read(reinterpret_cast<char*>(m_prg_rom.data()), m_prg_rom.size()))
    {
        LOG_ERROR("Error while reading PRG data from %s", file_path.c_str());
        return false;
    }

    if (chr_rom_size == 0)
    {
        m_use_chr_ram = true;
        m_chr_rom.resize(prg_ram_size);
    }
    else
    {
        m_chr_rom.resize(chr_rom_size);
        if (!rom_file.read(reinterpret_cast<char*>(m_chr_rom.data()), m_chr_rom.size()))
        {
            LOG_ERROR("Error while reading CHR data from %s", file_path.c_str());
            return false;
        }
    }

    LOG_DEBUG("Cartridge: Mapper: %u, PRG banks: %u, CHR banks: %u,"
              " PRG RAM: %u KB, PRG ROM: %u KB, CHR ROM: %u KB, Mirror mode: %s",
              m_mapper->id(), m_mapper->prg_banks(), m_mapper->chr_banks(),
              m_prg_ram.size(), m_prg_rom.size(), m_chr_rom.size(),
              m_mapper->mirroring_mode() == MIRROR_VERTICAL ? "Vertical" : "Horizontal");

    m_loaded = true;

    return true;
}

uint8_t Cartridge::cpu_read(uint16_t address)
{
    uint32_t mapped_address = m_mapper->read(address);

    if (address >= 0x6000 && address < 0x8000)
        return m_prg_ram[mapped_address];
    else if (address >= 0x8000)
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

    if (address < 0x2000 && m_use_chr_ram)
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

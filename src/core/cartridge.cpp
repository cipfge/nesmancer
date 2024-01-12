#include "cartridge.hpp"
#include "nes_file.hpp"
#include "nrom.hpp"
#include "mmc1.hpp"
#include "mmc3.hpp"
#include "uxrom.hpp"
#include "cnrom.hpp"
#include "axrom.hpp"
#include "logger.hpp"
#include <fstream>

void Cartridge::reset()
{
    m_prg_ram.fill(0x00);
    m_prg_rom.clear();
    m_chr_ram.fill(0x00);
    m_chr_rom.clear();
    m_mapper.reset();
    m_use_chr_ram = false;
    m_loaded = false;
}

bool Cartridge::load_from_file(const std::string& file_path)
{
    std::ifstream stream(file_path, std::ifstream::binary);
    if (!stream.is_open())
    {
        LOG_ERROR("Failed to open NES file %s", file_path.c_str());
        return false;
    }

    NesFile nes_file{};
    if (!nes_file.load(stream))
    {
        LOG_ERROR("Failed to read NES header from %s", file_path.c_str());
        return false;
    }

    if (!nes_file.valid())
    {
        LOG_ERROR("Invalid NES file header signature");
        return false;
    }

    if (nes_file.version() == NesFileVersion::Unsupported)
    {
        LOG_ERROR("Unsupported NES file version!");
        return false;
    }

    reset();

    switch (nes_file.mapper_id())
    {
    case MAPPER_NROM:
        m_mapper = std::make_shared<NROM>(nes_file.program_bank_count(),
                                          nes_file.character_bank_count(),
                                          nes_file.mirroring_mode());
        break;

    case MAPPER_MCC1:
        m_mapper = std::make_shared<MMC1>(nes_file.program_bank_count(),
                                          nes_file.character_bank_count(),
                                          nes_file.mirroring_mode());
        break;

    case MAPPER_UXROM:
        m_mapper = std::make_shared<UxROM>(nes_file.program_bank_count(),
                                           nes_file.character_bank_count(),
                                           nes_file.mirroring_mode());
        break;

    case MAPPER_CNROM:
        m_mapper = std::make_shared<CNROM>(nes_file.program_bank_count(),
                                           nes_file.character_bank_count(),
                                           nes_file.mirroring_mode());
        break;

    default:
        LOG_ERROR("Unsupported mapper id %u", nes_file.mapper_id());
        return false;
    }

    if (nes_file.trainer())
    {
        LOG_INFO("Skip trainer information, not supported.");
        stream.seekg(NesFile::TrainerSize, std::ios::cur);
    }

    if (nes_file.program_rom_size() == 0)
    {
        LOG_ERROR("PRG ROM size is 0");
        return false;
    }

    m_prg_rom.resize(nes_file.program_rom_size());
    if (!stream.read(reinterpret_cast<char*>(m_prg_rom.data()), m_prg_rom.size()))
    {
        LOG_ERROR("Error while reading PRG data from %s", file_path.c_str());
        return false;
    }

    if (nes_file.character_rom_size() == 0)
    {
        m_use_chr_ram = true;
    }
    else
    {
        m_use_chr_ram = false;
        m_chr_rom.resize(nes_file.character_rom_size());
        if (!stream.read(reinterpret_cast<char*>(m_chr_rom.data()), m_chr_rom.size()))
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
    {
        if (m_use_chr_ram)
            return m_chr_ram[mapped_address];
        else
            return m_chr_rom[mapped_address];
    }

    return m_vram[mapped_address];
}

void Cartridge::ppu_write(uint16_t address, uint8_t data)
{
    uint32_t mapped_address = m_mapper->write(address, data);

    if (address < 0x2000)
    {
        if (m_use_chr_ram)
            m_chr_ram[mapped_address] = data;
    }
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

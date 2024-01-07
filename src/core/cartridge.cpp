#include "cartridge.hpp"
#include "nrom.hpp"
#include "mmc1.hpp"
#include "mmc3.hpp"
#include "uxrom.hpp"
#include "cnrom.hpp"
#include "axrom.hpp"
#include "logger.hpp"
#include <fstream>
#include <cstring>

bool NesFileHeader::is_valid() const
{
    if (strncmp(signature, "NES\x1A", sizeof(signature)) == 0)
        return true;

    return false;
}

NesRomVersion NesFileHeader::get_version() const
{
    const uint8_t version_mask = 0x0C;

    if ((bytes[1] & version_mask) == 0x08)
        return NesRomVersion::Nes2;
    else if ((bytes[1] & version_mask) == 0x00)
        return NesRomVersion::iNes;

    return NesRomVersion::Unsupported;
}

bool NesFileHeader::has_trainer() const
{
    const uint8_t trainer_mask = 0x04;
    return (bytes[0] & trainer_mask) == trainer_mask;
}

uint16_t NesFileHeader::get_mapper_id() const
{
    switch (get_version())
    {
    case NesRomVersion::iNes:
        return (bytes[1] & 0xF0) | (bytes[0] >> 4);
    case NesRomVersion::Nes2:
        return ((bytes[2] & 0x0F) << 8) | (bytes[1] & 0xF0) | (bytes[0] >> 4);

    default:
        return 0xFFFF;
    }
}

MirroringMode NesFileHeader::get_mirroring_mode() const
{
    if (bytes[0] & 0x08)
        return MirroringMode::FourScreens;
    else
    {
        if (bytes[0] & 0x01)
            return MirroringMode::Vertical;
        else
            return MirroringMode::Horizontal;
    }
}

uint32_t NesFileHeader::get_prg_size() const
{
    switch (get_version())
    {
    case NesRomVersion::iNes:
        return prg_banks * 0x4000;
    case NesRomVersion::Nes2:
        return (((bytes[2] & 0x07) << 8) | prg_banks) * 0x4000;
    default:
        return 0;
    }
}

uint32_t NesFileHeader::get_chr_size() const
{
    switch (get_version())
    {
    case NesRomVersion::iNes:
        return chr_banks * 0x2000;
    case NesRomVersion::Nes2:
        return (((bytes[2] & 0x38) << 8) | chr_banks) * 0x2000;
    default:
        return 0;
    }
}

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
    std::ifstream rom_file(file_path, std::ifstream::binary);
    if (!rom_file.is_open())
    {
        LOG_ERROR("Failed to open NES file %s", file_path.c_str());
        return false;
    }

    NesFileHeader header{};
    if (!rom_file.read(reinterpret_cast<char*>(&header), sizeof(header)))
    {
        LOG_ERROR("Failed to read NES header from %s", file_path.c_str());
        return false;
    }

    if (!header.is_valid())
    {
        LOG_ERROR("Invalid NES header signature %s", header.signature);
        return false;
    }

    if (header.get_version() == NesRomVersion::Unsupported)
    {
        LOG_ERROR("Unsupported NES rom version!");
        return false;
    }

    reset();

    switch (header.get_mapper_id())
    {
    case MAPPER_NROM:
        m_mapper = std::make_shared<NROM>(header.prg_banks,
                                          header.chr_banks,
                                          header.get_mirroring_mode());
        break;

    case MAPPER_MCC1:
        m_mapper = std::make_shared<MMC1>(header.prg_banks,
                                          header.chr_banks,
                                          header.get_mirroring_mode());
        break;

    case MAPPER_UXROM:
        m_mapper = std::make_shared<UxROM>(header.prg_banks,
                                           header.chr_banks,
                                           header.get_mirroring_mode());
        break;

    case MAPPER_CNROM:
        m_mapper = std::make_shared<CNROM>(header.prg_banks,
                                           header.chr_banks,
                                           header.get_mirroring_mode());
        break;

    case MAPPER_MMC3:
        m_mapper = std::make_shared<MMC3>(header.prg_banks,
                                          header.chr_banks,
                                          header.get_mirroring_mode());
        break;

    case MAPPER_AXROM:
        m_mapper = std::make_shared<AxROM>(header.prg_banks,
                                           header.chr_banks,
                                           header.get_mirroring_mode());
        break;

    default:
        LOG_ERROR("Unsupported mapper id %u", header.get_mapper_id());
        return false;
    }

    if (header.has_trainer())
    {
        LOG_INFO("Skip trainer information, not supported.");
        rom_file.seekg(TrainerSize, std::ios::cur);
    }

    uint32_t prg_size = header.get_prg_size();
    if (prg_size == 0)
    {
        LOG_ERROR("PRG ROM size is 0");
        return false;
    }

    m_prg_rom.resize(prg_size);
    if (!rom_file.read(reinterpret_cast<char*>(m_prg_rom.data()), m_prg_rom.size()))
    {
        LOG_ERROR("Error while reading PRG data from %s", file_path.c_str());
        return false;
    }

    uint32_t chr_size = header.get_chr_size();
    if (chr_size == 0)
    {
        m_use_chr_ram = true;
    }
    else
    {
        m_use_chr_ram = false;
        m_chr_rom.resize(chr_size);
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

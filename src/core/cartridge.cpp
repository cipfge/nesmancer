#include "cartridge.hpp"
#include "global.hpp"
#include <fstream>
#include <cstring>
#include <iostream>

Cartridge::~Cartridge()
{
}

void Cartridge::reset()
{
    memset(m_vram, 0, sizeof(m_vram));

    m_info.mapper = 0;
    m_info.prg_banks = 0;
    m_info.chr_banks = 0;
    m_info.prg_ram_size = 0;
    m_info.prg_rom_size = 0;
    m_info.chr_rom_size = 0;
    m_info.mirror_mode = Mapper::MIRROR_HORIZONTAL;
    m_info.baterry = false;
    m_info.trainer = false;
    m_info.four_screen_mode = false;
    m_info.vs_unisystem = false;
    m_info.playchoice_10 = false;
    m_info.is_nes2_format = false;

    m_prg_ram.clear();
    m_prg_rom.clear();
    m_chr_rom.clear();

    m_mapper.reset();

    m_chr_ram = false;
    m_rom_loaded = false;
}

const Cartridge::Info& Cartridge::info() const
{
    return m_info;
}

bool Cartridge::rom_loaded() const
{
    return m_rom_loaded;
}

bool Cartridge::load_from_file(const std::string& rom_file_path)
{
    std::ifstream rom_file(rom_file_path, std::ifstream::binary);
    if (!rom_file.is_open())
    {
        std::cerr << "Failed to open rom file " << rom_file_path << "\n";
        return false;
    }

    uint8_t rom_header[16];
    if (!rom_file.read(reinterpret_cast<char*>(&rom_header), sizeof(rom_header)))
        return false;

    if (strncmp((char *)rom_header, "NES\x1A", 4) != 0)
    {
        std::cerr << "Invalid signature for file " << rom_file_path << "\n";
        return false;
    }

    reset();
    parse_rom_header(rom_header);

    switch (m_info.mapper)
    {
    case 0:
        m_mapper = std::make_shared<Mapper0>(m_info.prg_banks,
                                             m_info.chr_banks,
                                             m_info.mirror_mode);
        break;

    // TODO: Mapper1, Mapper2, ...

    default:
        std::cerr << "Unsupported mapper id " << (unsigned)m_info.mapper << "\n";
        return false;
    }

    m_prg_ram.resize(m_info.prg_ram_size);

    if (m_info.trainer)
        rom_file.seekg(512, std::ios::cur);

    m_prg_rom.resize(m_info.prg_rom_size);
    if (!rom_file.read(reinterpret_cast<char*>(m_prg_rom.data()), m_info.prg_rom_size))
    {
        std::cerr << "Error while reading PRG data from " << rom_file_path << "\n";
        return false;
    }

    if (m_info.chr_rom_size == 0)
        m_chr_ram = true;
    else
    {
        m_chr_rom.resize(m_info.chr_rom_size);
        if (!rom_file.read(reinterpret_cast<char*>(m_chr_rom.data()), m_info.chr_rom_size))
        {
            std::cerr << "Error while reading CHR data from " << rom_file_path << "\n";
            return false;
        }
    }

    m_rom_loaded = true;
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
    else
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

void Cartridge::parse_rom_header(const uint8_t* header)
{
    m_info.mapper = (header[6] >> 4) | (header[7] & 0xF0);
    m_info.prg_banks = header[4];
    m_info.chr_banks = header[5];
    m_info.prg_ram_size = header[8] ? header[8] * 8192 : 8192;
    m_info.prg_rom_size = static_cast<uint32_t>(header[4] << 14);
    m_info.chr_rom_size = static_cast<uint32_t>(header[5] << 13);
    m_info.mirror_mode = static_cast<Mapper::MirrorMode>(header[6] & 0x1);
    m_info.baterry = header[6] & 0x2;
    m_info.trainer = header[6] & 0x4;
    m_info.four_screen_mode = header[6] & 0x2;
    m_info.vs_unisystem = header[7] & 0x1;
    m_info.playchoice_10 = header[7] & 0x2;
    m_info.is_nes2_format = ((header[7] >> 2) & 0x3) == 2;
}

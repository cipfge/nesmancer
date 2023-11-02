#include "cartridge.hpp"
#include <fstream>
#include <cstring>
#include <iostream>

Cartridge::~Cartridge()
{
}

const Cartridge::Info& Cartridge::info() const
{
    return m_info;
}

bool Cartridge::load_from_file(const std::string& path)
{
    std::ifstream rom_file(path, std::ifstream::binary);
    if (!rom_file.is_open())
    {
        std::cerr << "Failed to open rom file " << path << "\n";
        return false;
    }

    uint8_t rom_header[16];
    if (!rom_file.read(reinterpret_cast<char*>(&rom_header), sizeof(rom_header)))
        return false;

    if (strncmp((char *)rom_header, "NES\x1A", 4) != 0)
    {
        std::cerr << "Invalid signature for file " << path << "\n";
        return false;
    }

    parse_rom_header(rom_header);

    if (m_info.trainer)
        rom_file.seekg(512, std::ios::cur);

    // TODO: read PRG & CHR

    return true;
}

void Cartridge::parse_rom_header(const uint8_t* header)
{
    m_info.mapper = (header[6] >> 4) | (header[7] & 0xF0);
    m_info.prg_banks = header[4];
    m_info.chr_banks = header[5];
    m_info.prg_size = static_cast<uint32_t>(header[4] << 14);
    m_info.chr_size = static_cast<uint32_t>(header[5] << 13);
    m_info.mirror_mode = static_cast<MirrorMode>(header[6] & 0x1);
    m_info.baterry = header[6] & 0x2;
    m_info.trainer = header[6] & 0x4;
    m_info.four_screen_mode = header[6] & 0x2;
    m_info.vs_unisystem = header[7] & 0x1;
    m_info.playchoice_10 = header[7] & 0x2;
    m_info.is_nes2_format = ((header[7] >> 2) & 0x3) == 2;

#ifdef EMU_DEBUG_ENABLED
    std::cout << "ROM information: \n"
              << " - Mapper: " << (unsigned)m_info.mapper << "\n"
              << " - PRG banks: " << (unsigned)m_info.prg_banks << "\n"
              << " - CHR banks: " << (unsigned)m_info.chr_banks << "\n"
              << " - PRG size: " << m_info.prg_size << "\n"
              << " - CHR size: " << m_info.chr_size << "\n"
              << " - Mirror: " << mirror_mode_to_string(m_info.mirror_mode) << "\n"
              << " - Battery: " << (m_info.baterry ? "yes" : "no") << "\n"
              << " - Trainer: " << (m_info.trainer ? "yes" : "no") << "\n"
              << " - Four screen mode: " << (m_info.four_screen_mode ? "yes" : "no") << "\n"
              << " - VS Unisystem: " << (m_info.vs_unisystem ? "yes" : "no") << "\n"
              << " - PlayChoice-10: " << (m_info.playchoice_10 ? "yes" : "no") << "\n"
              << " - NES 2.0: " << (m_info.is_nes2_format ? "yes" : "no") << "\n";
#endif // Debug enabled
}

std::string Cartridge::mirror_mode_to_string(MirrorMode mode)
{
    switch (mode)
    {
    case MIRROR_HORIZONTAL: return "Horizontal";
    case MIRROR_VERTICAL: return "Vertical";
    default: return "Unknown";
    }

    return "";
}

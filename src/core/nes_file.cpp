#include "nes_file.hpp"
#include <cstring>

bool NesFile::load(std::ifstream& stream)
{
    if (!stream.read(reinterpret_cast<char*>(&m_header), sizeof(m_header)))
        return false;

    return true;
}

bool NesFile::valid()
{
    if (strncmp(m_header.signature, "NES\x1A", sizeof(m_header.signature)) != 0)
        return false;

    return true;
}

bool NesFile::trainer() const
{
    const uint8_t mask = 0x04;
    return (m_header.bytes[0] & mask) == mask;
}

NesFileVersion NesFile::version() const
{
    const uint8_t mask = 0x0C;

    if ((m_header.bytes[1] & mask) == 0x08)
        return NesFileVersion::Nes2;
    else if ((m_header.bytes[1] & mask) == 0x00)
        return NesFileVersion::Nes1;

    return NesFileVersion::Unsupported;
}

uint16_t NesFile::mapper_id() const
{
    switch (version())
    {
    case NesFileVersion::Nes1:
        return (m_header.bytes[1] & 0xF0) | (m_header.bytes[0] >> 4);
    case NesFileVersion::Nes2:
        return ((m_header.bytes[2] & 0x0F) << 8) | (m_header.bytes[1] & 0xF0) | (m_header.bytes[0] >> 4);
    default:
        return 0xFFFF;
    }
}

MirroringMode NesFile::mirroring_mode() const
{
    if (m_header.bytes[0] & 0x08)
        return MirroringMode::FourScreens;
    else
    {
        if (m_header.bytes[0] & 0x01)
            return MirroringMode::Vertical;
        else
            return MirroringMode::Horizontal;
    }
}

uint32_t NesFile::program_rom_size() const
{
    switch (version())
    {
    case NesFileVersion::Nes1:
        return m_header.program_bank_count * 0x4000;
    case NesFileVersion::Nes2:
        return (((m_header.bytes[2] & 0x07) << 8) | m_header.program_bank_count) * 0x4000;
    default:
        return 0;
    }
}

uint32_t NesFile::character_rom_size() const
{
    switch (version())
    {
    case NesFileVersion::Nes1:
        return m_header.character_bank_count * 0x2000;
    case NesFileVersion::Nes2:
        return (((m_header.bytes[2] & 0x38) << 8) | m_header.character_bank_count) * 0x2000;
    default:
        return 0;
    }
}

#include "nes_rom.hpp"
#include "platform.hpp"
#include <fstream>

NesRom::NesRom(const std::string& file_path)
{
    size_t file_size = platform::file_size(file_path);
    m_data.resize(file_size);

    std::ifstream fstream(file_path, std::ifstream::binary);
    if (!fstream.is_open())
        throw NesFileOpenException();

    m_data.insert(m_data.begin(),
                  std::istreambuf_iterator<char>(fstream),
                  std::istreambuf_iterator<char>());

    m_header = reinterpret_cast<NesFileHeader*>(&m_data[0]);
    if (!is_valid())
        throw NesInvalidRomException();

    if (version() == NesFileVersion::Unsupported)
        throw NesUnsupportedException();
}

bool NesRom::is_valid() const
{
    if (!m_header || m_data.size() == 0)
        return false;

    return strncmp(m_header->signature, NesFileHeader::ValidSignature, sizeof(m_header->signature)) == 0;
}

bool NesRom::has_trainer_data() const
{
    const uint8_t mask = 0x04;
    return (m_header->bytes[0] & mask) == mask;
}

NesFileVersion NesRom::version() const
{
    const uint8_t mask = 0x0C;

    if ((m_header->bytes[1] & mask) == 0x08)
        return NesFileVersion::Nes2;
    else if ((m_header->bytes[1] & mask) == 0x00)
        return NesFileVersion::iNes;

    return NesFileVersion::Unsupported;
}

uint16_t NesRom::mapper_id() const
{
    switch (version())
    {
    case NesFileVersion::iNes:
        return (m_header->bytes[1] & 0xF0) | (m_header->bytes[0] >> 4);
    case NesFileVersion::Nes2:
        return ((m_header->bytes[2] & 0x0F) << 8) | (m_header->bytes[1] & 0xF0) | (m_header->bytes[0] >> 4);
    default:
        return 0xFFFF;
    }
}
uint8_t NesRom::program_banks() const
{
    return m_header->prg_bank_count;
}

uint8_t NesRom::character_banks() const
{
    return m_header->chr_bank_count;
}

uint32_t NesRom::program_rom_size() const
{
    switch (version())
    {
    case NesFileVersion::iNes:
        return program_banks() * 0x4000;
    case NesFileVersion::Nes2:
        return (((m_header->bytes[2] & 0x07) << 8) | program_banks()) * 0x4000;
    default:
        return 0;
    }
}

uint32_t NesRom::program_ram_size() const
{
    return m_header->bytes[2] ? m_header->bytes[2] * 0x2000 : 0x2000;
}

uint32_t NesRom::character_rom_size() const
{
    switch (version())
    {
    case NesFileVersion::iNes:
        return character_banks() * 0x2000;
    case NesFileVersion::Nes2:
        return (((m_header->bytes[2] & 0x38) << 8) | character_banks()) * 0x2000;
    default:
        return 0;
    }
}

MirroringMode NesRom::mirroring_mode() const
{
    if (m_header->bytes[0] & 0x08)
        return MirroringMode::FourScreens;
    else
    {
        if (m_header->bytes[0] & 0x01)
            return MirroringMode::Vertical;
        else
            return MirroringMode::Horizontal;
    }
}

void NesRom::read_prg_data(std::vector<uint8_t>& buffer)
{
    uint32_t offset = sizeof(NesFileHeader);
    if (has_trainer_data())
        offset += NesFileHeader::TrainerSize;

    std::copy(m_data.begin() + offset, m_data.begin() + (offset + program_rom_size()), buffer.begin());
}

void NesRom::read_chr_data(std::vector<uint8_t>& buffer)
{
    if (character_rom_size() == 0)
        return;

    uint32_t offset = sizeof(NesFileHeader);
    if (has_trainer_data())
        offset += NesFileHeader::TrainerSize;
    offset += program_rom_size();

    std::copy(m_data.begin() + offset, m_data.begin() + (offset + character_rom_size()), buffer.begin());
}

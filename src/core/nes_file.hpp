#pragma once

#include "types.hpp"
#include <cstdint>
#include <fstream>

enum class NesFileVersion
{
    Nes1,
    Nes2,
    Unsupported
};

class NesFile
{
public:
    bool load(std::ifstream& stream);

    uint8_t program_bank_count() const
    {
        return m_header.program_bank_count;
    }

    uint8_t character_bank_count() const
    {
        return m_header.character_bank_count;
    }

    bool valid();
    bool trainer() const;
    NesFileVersion version() const;
    uint16_t mapper_id() const;
    MirroringMode mirroring_mode() const;
    uint32_t program_rom_size() const;
    uint32_t program_ram_size() const;
    uint32_t character_rom_size() const;

    static constexpr uint16_t TrainerSize = 512;

private:
    struct Header
    {
        char signature[4] = {};
        uint8_t program_bank_count = 0;
        uint8_t character_bank_count = 0;
        uint8_t bytes[10] = {};
    };

    Header m_header{};
};

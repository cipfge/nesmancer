#pragma once

#include "nes_file.hpp"
#include "types.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

class NesFileOpenException : public std::runtime_error
{
public:
    NesFileOpenException():
        runtime_error("Cannot open file")
    {}
};

class NesInvalidRomException : public std::runtime_error
{
public:
    NesInvalidRomException():
        runtime_error("Invalid NES rom file")
    {}
};

class NesUnsupportedException : public std::runtime_error
{
public:
    NesUnsupportedException() :
        runtime_error("Unsupported NES rom version")
    {}
};

class NesRom
{
public:
    NesRom(const std::string& file_path);

    bool is_valid() const;
    bool has_trainer_data() const;
    NesFileVersion version() const;
    uint16_t mapper_id() const;
    uint8_t program_banks() const;
    uint8_t character_banks() const;
    uint32_t program_rom_size() const;
    uint32_t program_ram_size() const;
    uint32_t character_rom_size() const;
    MirroringMode mirroring_mode() const;

    void read_prg_data(std::vector<uint8_t>& buffer);
    void read_chr_data(std::vector<uint8_t>& buffer);

private:
    std::vector<uint8_t> m_data;
    NesFileHeader* m_header = nullptr;
};

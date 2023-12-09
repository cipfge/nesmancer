#pragma once

#include "mapper.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

enum class NesRomVersion
{
    iNes,
    Nes2,
    Unsupported
};

struct NesFileHeader
{
    char signature[4];
    uint8_t prg_banks;
    uint8_t chr_banks;
    uint8_t bytes[10];

    bool is_valid() const;
    NesRomVersion get_version() const;
    bool has_trainer() const;
    uint16_t get_mapper_id() const;
    MirroringMode get_mirroring_mode() const;
    uint32_t get_prg_size() const;
    uint32_t get_chr_size() const;
};

class Cartridge
{
public:
    Cartridge() = default;
    ~Cartridge() = default;

    void reset();
    bool load_from_file(const std::string& file_path);

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);

    bool irq();
    void irq_clear();
    void scanline();

    bool is_loaded() const { return m_loaded; }

    static constexpr uint16_t TrainerSize = 512;

private:
    std::array<uint8_t, 0x4000> m_prg_ram;
    std::vector<uint8_t> m_prg_rom;
    std::array<uint8_t, 0x4000> m_chr_ram;
    std::vector<uint8_t> m_chr_rom;
    std::array<uint8_t, 0x800> m_vram;
    std::shared_ptr<Mapper> m_mapper = nullptr;

    bool m_use_chr_ram = false;
    bool m_loaded = false;
};

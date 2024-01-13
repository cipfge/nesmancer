#pragma once

#include "mapper.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

class Cartridge
{
public:
    Cartridge() = default;
    ~Cartridge() = default;

    void reset();
    bool load_from_file(const std::string& file_path);
    bool loaded() const { return m_loaded; }

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);

    bool irq();
    void irq_clear();
    void scanline();

private:
    std::unique_ptr<Mapper> m_mapper = nullptr;

    bool m_use_character_ram = false;
    bool m_loaded = false;

    std::array<uint8_t, 0x4000> m_program_ram;
    std::array<uint8_t, 0x4000> m_character_ram;
    std::array<uint8_t, 0x800> m_video_ram;

    std::vector<uint8_t> m_program_rom;
    std::vector<uint8_t> m_character_rom;
};

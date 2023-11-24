#pragma once

#include <cstdint>
#include "mapper.hpp"
#include <string>
#include <vector>
#include <memory>

class Cartridge
{
public:
    Cartridge() = default;
    ~Cartridge();

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

private:
    std::vector<uint8_t> m_prg_ram;
    std::vector<uint8_t> m_prg_rom;
    std::vector<uint8_t> m_chr_rom;
    uint8_t m_vram[2048];

    uint8_t m_mapper_id = 0;
    uint8_t m_prg_banks = 0;
    uint8_t m_chr_banks = 0;
    uint32_t m_prg_ram_size = 0;
    uint32_t m_prg_rom_size = 0;
    uint32_t m_chr_rom_size = 0;
    MirroringMode m_mirroring_mode = MIRROR_HORIZONTAL;

    std::shared_ptr<Mapper> m_mapper = nullptr;

    bool m_has_trainer = false;
    bool m_use_chr_ram = false;
    bool m_loaded = false;

    void parse_nes_header(const uint8_t* header);
};

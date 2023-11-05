#pragma once

#include "types.hpp"
#include "mapper.hpp"
#include <vector>
#include <memory>

class Cartridge
{
public:
    struct Info
    {
        uint8_t mapper = 0;
        uint8_t prg_banks = 0;
        uint8_t chr_banks = 0;
        uint32_t prg_ram_size = 0;
        uint32_t prg_rom_size = 0;
        uint32_t chr_rom_size = 0;
        Mapper::MirrorMode mirror_mode = Mapper::MIRROR_HORIZONTAL;
        bool baterry = false;
        bool trainer = false;
        bool four_screen_mode = false;
        bool vs_unisystem = false;
        bool playchoice_10 = false;
        bool is_nes2_format = false;
    };

public:
    Cartridge() = default;
    ~Cartridge();

    void reset();

    const Info& info() const;
    bool rom_loaded() const;
    bool load_from_file(const std::string& rom_file_path);

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);

private:
    Info m_info;
    bool m_chr_ram = false;
    bool m_rom_loaded = false;
    uint8_t m_vram[2048];
    std::vector<uint8_t> m_prg_ram;
    std::vector<uint8_t> m_prg_rom;
    std::vector<uint8_t> m_chr_rom;
    std::shared_ptr<Mapper> m_mapper = nullptr;

    void parse_rom_header(const uint8_t* header);
};

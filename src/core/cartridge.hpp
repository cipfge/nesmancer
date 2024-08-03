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

    void reset();
    bool load_from_file(const std::string& file_path);
    bool loaded() const { return m_mapper != nullptr; }
    MirroringMode mirroring_mode();

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);

    bool irq();
    void irq_clear();
    void scanline();

private:
    std::unique_ptr<Mapper> m_mapper = nullptr;
};

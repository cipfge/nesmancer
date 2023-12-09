#pragma once

#include <cstdint>
#include <array>

class APU;
class PPU;
class Cartridge;
class Controller;

class Memory
{
public:
    Memory(APU& apu,
           PPU& ppu,
           Cartridge& cartridge,
           Controller& controller);
    ~Memory();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    std::array<uint8_t, 0x800> m_ram;
    APU& m_apu;
    PPU& m_ppu;
    Cartridge& m_cartrige;
    Controller& m_controller;

    void oam_dma(uint8_t data);
};

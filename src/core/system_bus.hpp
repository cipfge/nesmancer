#pragma once

#include <cstdint>
#include <array>

class CPU;
class APU;
class PPU;
class Cartridge;
class Controller;

class SystemBus
{
public:
    SystemBus(APU& apu, PPU& ppu, Cartridge& cartridge, Controller& controller):
        m_apu(apu),
        m_ppu(ppu),
        m_cartrige(cartridge),
        m_controller(controller)
    {}

    void set_cpu(CPU* cpu) { m_cpu = cpu; }

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    std::array<uint8_t, 0x800> m_ram{};
    CPU* m_cpu = nullptr;
    APU& m_apu;
    PPU& m_ppu;
    Cartridge& m_cartrige;
    Controller& m_controller;

    void oam_dma(uint8_t data);
};

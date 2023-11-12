#pragma once

#include <cstdint>

class APU;
class PPU;
class Cartridge;
class Controller;

class Memory
{
public:
    Memory(APU* apu,
           PPU* ppu,
           Cartridge* cartridge,
           Controller* controller);
    ~Memory();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    uint8_t m_internal_ram[2048];
    APU* m_apu = nullptr;
    PPU* m_ppu = nullptr;
    Cartridge* m_cartrige = nullptr;
    Controller* m_controller = nullptr;

    void oam_dma(uint8_t data);
};

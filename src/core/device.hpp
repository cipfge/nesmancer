#pragma once

#include <cstdint>
#include <string>

class CPU;
class APU;
class PPU;
class Cartridge;
class Controller;
class Memory;

class Device
{
public:
    Device();
    ~Device();

    void reset();
    void run();
    bool load_rom_file(const std::string& file_path);
    uint32_t* screen();

private:
    CPU* m_cpu = nullptr;
    APU* m_apu = nullptr;
    PPU* m_ppu = nullptr;
    Cartridge* m_cartridge = nullptr;
    Controller* m_controller = nullptr;
    Memory* m_memory = nullptr;
};

#pragma once

#include "types.hpp"
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
    void run_one_frame();
    bool load_rom(const std::string rom_file_path);

    uint32_t* frame_buffer();

private:
    CPU* m_cpu = nullptr;
    APU* m_apu = nullptr;
    PPU* m_ppu = nullptr;
    Cartridge* m_cartridge = nullptr;
    Controller* m_controller = nullptr;
    Memory* m_memory = nullptr;
};

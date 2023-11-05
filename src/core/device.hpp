#pragma once

#include "types.hpp"
#include <string>

class CPU;
class PPU;
class Cartridge;
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
    PPU* m_ppu = nullptr;
    Cartridge* m_cartridge = nullptr;
    Memory* m_memory = nullptr;
};

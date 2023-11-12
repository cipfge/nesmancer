#pragma once

#include <cstdint>
#include <string>

class CPU;
class APU;
class PPU;
class Cartridge;
class Controller;
class Memory;

enum Button
{
    BUTTON_RIGHT = (1 << 0),
    BUTTON_LEFT = (1 << 1),
    BUTTON_DOWN = (1 << 2),
    BUTTON_UP = (1 << 3),
    BUTTON_START = (1 << 4),
    BUTTON_SELECT = (1 << 5),
    BUTTON_B = (1 << 6),
    BUTTON_A = (1 << 7),
};

class Device
{
public:
    Device();
    ~Device();

    void reset();
    void run();
    bool load_rom_file(const std::string& file_path);
    void set_button_state(Button btn, bool pressed);
    uint32_t* screen();

private:
    CPU* m_cpu = nullptr;
    APU* m_apu = nullptr;
    PPU* m_ppu = nullptr;
    Cartridge* m_cartridge = nullptr;
    Controller* m_controller = nullptr;
    Memory* m_memory = nullptr;
};

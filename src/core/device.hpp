#pragma once

#include <cstdint>
#include <string>

class CPU;
class APU;
class PPU;
class Cartridge;
class Controller;
class Memory;
class InputManager;

class Device
{
public:
    Device();
    ~Device();

    bool init();
    void set_input_manager(InputManager* input_manager);
    void reset();
    void power_off();
    void run();
    bool load_rom_file(const std::string& file_path);
    bool is_running() const;
    bool is_paused() const { return m_paused; }
    void toggle_pause();
    uint32_t* screen();

private:
    CPU* m_cpu = nullptr;
    APU* m_apu = nullptr;
    PPU* m_ppu = nullptr;
    Cartridge* m_cartridge = nullptr;
    Controller* m_controller = nullptr;
    Memory* m_memory = nullptr;
    bool m_paused = false;
};

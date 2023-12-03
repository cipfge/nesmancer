#pragma once

#include "cpu.hpp"
#include "apu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "controller.hpp"
#include "memory.hpp"
#include <cstdint>
#include <string>

class InputManager;

class Device
{
public:
    Device(InputManager& input_manager);
    ~Device();

    bool init();
    void reset();
    void power_off();
    void run();
    bool load_rom_file(const std::string& file_path);
    bool is_running() const;
    bool is_paused() const { return m_paused; }
    void toggle_pause();
    uint32_t* get_screen_buffer();

private:
    Cartridge m_cartridge;
    CPU m_cpu;
    APU m_apu;
    PPU m_ppu;
    Controller m_controller;
    Memory m_memory;
    bool m_paused = false;
};

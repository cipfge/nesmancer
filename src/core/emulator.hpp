#pragma once

#include "cpu.hpp"
#include "apu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "controller.hpp"
#include "system_bus.hpp"
#include <cstdint>
#include <string>

class InputManager;

class Emulator
{
public:
    Emulator(InputManager& input_manager):
        m_ppu(m_cartridge),
        m_controller(input_manager),
        m_system_bus(m_apu, m_ppu, m_cartridge, m_controller),
        m_cpu(m_system_bus)
    {}

    ~Emulator() = default;

    bool init();
    void reset();
    void power_off();
    void run();
    bool load_rom_file(const std::string& file_path);
    bool running() const { return m_cartridge.loaded(); }
    bool paused() const { return m_paused; }
    void toggle_pause();
    uint32_t* screen_buffer() { return m_ppu.frame_buffer(); }

private:
    Cartridge m_cartridge;
    CPU m_cpu;
    APU m_apu;
    PPU m_ppu;
    Controller m_controller;
    SystemBus m_system_bus;
    bool m_paused = false;
};

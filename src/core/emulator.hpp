#pragma once

#include "cpu.hpp"
#include "apu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "controller.hpp"
#include "system_bus.hpp"
#include "sound_queue.hpp"
#include <cstdint>
#include <string>

class InputManager;

class Emulator
{
public:
    Emulator(InputManager& input_manager);

    bool init();
    void reset();
    void power_off();
    void run();
    bool load_rom_file(const std::string& file_path);
    bool running() const { return m_cartridge.loaded(); }
    bool paused() const { return m_paused; }
    void toggle_pause();

    const CPU& cpu() { return m_cpu; }
    const PPU& ppu() { return m_ppu; }
    uint32_t* screen_buffer() { return m_ppu.frame_buffer(); }

private:
    Cartridge m_cartridge;
    CPU m_cpu;
    APU m_apu;
    PPU m_ppu;
    Controller m_controller;
    SystemBus m_system_bus;
    std::unique_ptr<SoundQueue> m_sound_queue = nullptr;
    blip_sample_t m_sound_buffer[APU::SoundBufferSize] = {};
    bool m_paused = false;
};

#include "emulator.hpp"
#include "input_manager.hpp"
#include "logger.hpp"
#include <iostream>
#include <fstream>

Emulator::Emulator(InputManager& input_manager):
    m_ppu(m_cartridge),
    m_controller(input_manager),
    m_system_bus(m_apu, m_ppu, m_cartridge, m_controller),
    m_cpu(m_system_bus)
{
    m_system_bus.set_cpu(&m_cpu);
    m_apu.set_system_bus(&m_system_bus);
}

bool Emulator::init()
{
    return m_apu.init();
}

void Emulator::reset()
{
    if (!m_cartridge.loaded())
        return;

    if (m_paused)
        m_paused = false;

    m_apu.reset();
    m_ppu.reset();
    m_cpu.reset();
}

void Emulator::power_off()
{
    m_cartridge.reset();
}

void Emulator::run()
{
    if (!m_cartridge.loaded())
        return;

    if (m_paused)
        return;

    m_ppu.frame_start();

    while (!m_ppu.frame_rendered())
    {
        // PPU is 3 times faster
        m_ppu.tick();
        m_ppu.tick();
        m_ppu.tick();
        m_cpu.tick();

        if (m_ppu.nmi())
        {
            m_cpu.nmi();
            m_ppu.nmi_clear();
        }

        if (m_cartridge.irq())
        {
            m_cpu.irq();
            m_cartridge.irq_clear();
        }
    }

    m_apu.end_frame();
}

bool Emulator::load_rom_file(const std::string& file_path)
{
    if (!m_cartridge.load_from_file(file_path))
        return false;

    reset();

    return true;
}

bool Emulator::load_palette_file(const std::string& file_path)
{
    std::fstream stream(file_path, std::ios::in | std::ios::binary);
    if (!stream.is_open())
        return false;

    uint32_t palette[64];
    stream.read(reinterpret_cast<char*>(&palette), sizeof(palette));
    m_ppu.set_palette(palette);
    LOG_INFO("Palette loaded from %s", file_path.c_str());

    return true;
}

const long Emulator::sound_samples_available() const
{
    return m_apu.samples_available();
}

const long Emulator::read_sound_samples(blip_sample_t* buffer, long size)
{
    assert(buffer);
    return m_apu.read_samples(buffer, size);
}

void Emulator::toggle_pause()
{
    if (m_cartridge.loaded())
        m_paused = !m_paused;
}

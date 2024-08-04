#include "emulator.hpp"
#include "input_manager.hpp"
#include "logger.hpp"

Emulator::Emulator(InputManager& input_manager):
    m_ppu(m_cartridge),
    m_controller(input_manager),
    m_system_bus(m_apu, m_ppu, m_cartridge, m_controller),
    m_cpu(m_system_bus)
{
    m_apu.set_system_bus(&m_system_bus);
}

bool Emulator::init()
{
    m_sound_queue = std::make_unique<SoundQueue>();
    if (!m_sound_queue->init(APU::SoundSampleRate))
        return false;

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
        if (m_cpu.cycles() == 0 && m_ppu.nmi())
        {
            m_cpu.nmi();
            m_ppu.nmi_clear();
        }

        if (m_cpu.cycles() == 0 && m_cartridge.irq())
        {
            m_cpu.irq();
            m_cartridge.irq_clear();
        }

        m_cpu.tick();

        // PPU is 3 times faster
        m_ppu.tick();
        m_ppu.tick();
        m_ppu.tick();
    }

    m_apu.end_frame();

    if (m_apu.samples_available() >= APU::SoundBufferSize) {
        long size = m_apu.read_samples(m_sound_buffer, sizeof(m_sound_buffer) / sizeof(blip_sample_t));
        m_sound_queue->write(m_sound_buffer, size);
    }
}

bool Emulator::load_rom_file(const std::string& file_path)
{
    if (!m_cartridge.load_from_file(file_path))
        return false;

    reset();

    return true;
}

void Emulator::toggle_pause()
{
    if (m_cartridge.loaded())
        m_paused = !m_paused;
}

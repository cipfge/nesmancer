#include "emulator.hpp"
#include "input_manager.hpp"
#include "logger.hpp"

bool Emulator::init()
{
    m_sound_queue = std::make_unique<Sound_Queue>();

    if (m_sound_queue->init(SoundSampleRate))
        return false;

    if (m_apu.set_sample_rate(SoundSampleRate))
        return false;

    return true;
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

        for (uint8_t i = 0; i < 3; i++)
            m_ppu.tick();

        long size = m_apu.read_samples(m_sound_buffer, sizeof(m_sound_buffer) / sizeof(blip_sample_t));
        m_sound_queue->write(m_sound_buffer, size);
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

void Emulator::toggle_pause()
{
    if (m_cartridge.loaded())
        m_paused = !m_paused;
}

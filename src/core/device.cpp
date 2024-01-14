#include "device.hpp"
#include "input_manager.hpp"
#include "logger.hpp"

bool Device::init()
{
    return m_apu.init_audio_device();
}

void Device::reset()
{
    if (!m_cartridge.loaded())
        return;

    if (m_paused)
        m_paused = false;

    m_apu.reset();
    m_ppu.reset();
    m_cpu.reset();
}

void Device::power_off()
{
    m_cartridge.reset();
}

void Device::run()
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
    }
}

bool Device::load_rom_file(const std::string& file_path)
{
    if (!m_cartridge.load_from_file(file_path))
        return false;

    reset();

    return true;
}

void Device::toggle_pause()
{
    if (m_cartridge.loaded())
        m_paused = !m_paused;
}

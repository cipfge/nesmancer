#include "device.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "memory.hpp"
#include "global.hpp"

Device::Device()
{
    m_cartridge = new Cartridge();
    m_ppu = new PPU(m_cartridge);
    m_memory = new Memory(m_ppu, m_cartridge);
    m_cpu = new CPU(m_memory);
}

Device::~Device()
{
    delete m_ppu;
    delete m_cartridge;
    delete m_memory;
    delete m_cpu;
}

void Device::reset()
{
    m_cpu->reset();
    m_ppu->reset();
    // TODO: APU reset
}

void Device::run_one_frame()
{
    if (!m_cartridge->rom_loaded())
        return;

    while (!m_ppu->frame_completed())
    {
        if (m_ppu->cpu_nmi())
        {
            m_cpu->nmi_pending();
            m_ppu->cpu_nmi_clear();
        }

        if (m_cartridge->cpu_irq())
            m_cpu->irq_pending();

        m_cpu->tick();

        // TODO: APU tick

        m_ppu->tick();
        m_ppu->tick();
        m_ppu->tick();
    }

    m_ppu->frame_clear();
}

bool Device::load_rom(const std::string rom_file_path)
{
    if (!m_cartridge->load_from_file(rom_file_path))
        return false;
    reset();

    return true;
}

uint32_t* Device::frame_buffer()
{
    return m_ppu->frame_buffer();
}

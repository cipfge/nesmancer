#include "device.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "memory.hpp"
#include "global.hpp"

Device::Device()
{
    m_ppu = new PPU();
    m_cartridge = new Cartridge();
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
    // TODO: APU, PPU reset
}

void Device::run_one_frame()
{
    if (!m_cartridge->rom_loaded())
        return;

    // TODO: PPU, check frame completed
    for (int cycle = 0; cycle < 29780; cycle++)
    {
        m_cpu->tick();
        // TODO: APU, PPU tick
    }
}

bool Device::load_rom(const std::string rom_file_path)
{
    if (!m_cartridge->load_from_file(rom_file_path))
        return false;

    reset();
    return true;
}

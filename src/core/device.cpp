#include "device.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "memory.hpp"

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

bool Device::load_rom(const std::string rom_file_path)
{
    return m_cartridge->load_from_file(rom_file_path);
}

#include "memory.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"

Memory::Memory(PPU* ppu, Cartridge* cartridge)
    : m_ppu(ppu)
    , m_cartrige(cartridge)
{
}

Memory::~Memory()
{
}

uint8_t Memory::read(uint16_t address)
{
    if (address < 0x2000)
        return m_internal_ram[address & 0x7FF];
    else if (address < 0x4000)
        return 0; // TODO: PPU
    else if (address < 0x4016)
        return 0; // TODO: APU
    else if (address < 0x4020)
        return 0; // TODO: Controller
    else
        return m_cartrige->cpu_read(address);

    return 0;
}

void Memory::write(uint16_t address, uint8_t data)
{
    if (address < 0x2000)
        m_internal_ram[address & 0x7FF] = data;
    else if (address < 0x4000)
    {
        // TODO: PPU
    }
    else if (address < 0x4020)
    {
        // TODO: OAM-DMA, APU, Controller
    }
    else
        m_cartrige->cpu_write(address, data);
}

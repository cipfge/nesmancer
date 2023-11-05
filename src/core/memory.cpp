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
        return m_ppu->read((address - 0x2000) & 0x7);
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
        m_ppu->write((address - 0x2000) & 0x7, data);
    else if (address < 0x4020)
    {
        switch (address)
        {
        case 0x4014:
        {
            for (uint16_t i = 0; i < 256; i++)
                write(0x2004, read(0x100 * data + i));
            break;
        }

        case 0x4016:
            // TODO: Controller 0 write
            break;

        case 0x4017:
            // TODO: Controller 1 write
            break;

        default:
            // TODO: APU write
            break;
        }
    }
    else
        m_cartrige->cpu_write(address, data);
}

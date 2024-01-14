#include "memory.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "cartridge.hpp"
#include "controller.hpp"

uint8_t Memory::read(uint16_t address)
{
    if (address < 0x2000)
        return m_ram[address & 0x7FF];
    else if (address < 0x4000)
        return m_ppu.read(address);
    else if (address < 0x4016)
        return m_apu.read(address);
    else if (address < 0x4020)
    {
        switch (address)
        {
        case 0x4015: return m_apu.read(address);
        case 0x4016: return m_controller.read(0);
        case 0x4017: return m_controller.read(1);
        default:
            break;
        }
    }
    else
        return m_cartrige.cpu_read(address);

    return 0;
}

void Memory::write(uint16_t address, uint8_t data)
{
    if (address < 0x2000)
        m_ram[address & 0x7FF] = data;
    else if (address < 0x4000)
        m_ppu.write(address, data);
    else if (address < 0x4020)
    {
        switch (address)
        {
        case 0x4014:
            oam_dma(data);
            break;

        case 0x4016:
            m_controller.write(data);
            break;

        default:
            m_apu.write(address, data);
            break;
        }
    }
    else
        m_cartrige.cpu_write(address, data);
}

inline void Memory::oam_dma(uint8_t data)
{
    for (uint16_t i = 0; i < 256; i++)
        write(0x2004, read(0x100 * data + i));
}

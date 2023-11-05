#include "ppu.hpp"
#include "global.hpp"
#include <cstdlib>
#include <ctime>

// TODO: Generate or load from file
uint32_t PPU::m_palette[64] = {
    0x464646, 0x000154, 0x000070, 0x07006b,
    0x280048, 0x3c000e, 0x3e0000, 0x2c0000,
    0x0d0300, 0x001500, 0x001f00, 0x001f00,
    0x001420, 0x000000, 0x000000, 0x000000,
    0x9d9d9d, 0x0041b0, 0x1825d5, 0x4a0dcf,
    0x75009f, 0x900153, 0x920f00, 0x7b2800,
    0x514400, 0x205c00, 0x006900, 0x006916,
    0x005a6a, 0x000000, 0x000000, 0x000000,
    0xfeffff, 0x4896ff, 0x626dff, 0x8e5bff,
    0xd45eff, 0xf160b4, 0xf36f5e, 0xdc8817,
    0xb2a400, 0x7fbd00, 0x53ca28, 0x38ca76,
    0x36bbcb, 0x2b2b2b, 0x000000, 0x000000,
    0xfeffff, 0xb0d2ff, 0xb6bbff, 0xcbb4ff,
    0xedbcff, 0xf9bde0, 0xfac3bd, 0xf0ce9f,
    0xdfd990, 0xcae393, 0xb8e9a6, 0xade9c6,
    0xace3e9, 0xa7a7a7, 0x000000, 0x000000
};

PPU::PPU()
{
}

PPU::~PPU()
{
}

void PPU::reset()
{
    m_oam_address = 0;
    m_control.value = 0;
    m_mask.value = 0;
    m_status.value = 0;
    m_vram_address.value = 0;
    m_vram_temp_address.value = 0;
    m_data_buffer = 0;
    m_cycle = 0;
    m_scanline = 0;
    m_frame_completed = false;
}

void PPU::frame_clear()
{
    m_frame_completed = false;
}

bool PPU::frame_completed()
{
    return m_frame_completed;
}

void PPU::tick()
{
    // Fake noise
    m_frame_buffer[(m_scanline * EMU_SCREEN_HEIGHT + m_cycle) %
                   (EMU_SCREEN_WIDTH * EMU_SCREEN_HEIGHT)] =
                   m_palette[(std::rand() % 2) ? 0x3F : 0x30];

    m_cycle++;
    if (m_cycle >= 340)
    {
        m_cycle = 0;
        m_scanline++;
        if (m_scanline >= 260)
        {
            m_scanline = 0;
            m_frame_completed = true;
        }
    }
}

uint8_t PPU::read(uint16_t address)
{
    uint8_t data = 0;

    switch (address)
    {
    case PPU_CONTROL:
        break;
    case PPU_MASK:
        break;
    case PPU_STATUS:
        break;
    case PPU_OAM_ADDRESS:
        break;
    case PPU_OAM_DATA:
        break;
    case PPU_SCROLL:
        break;
    case PPU_ADDRESS:
        break;
    case PPU_DATA:
        break;
    default:
        break;
    }

    return data;
}

void PPU::write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(data);

    switch (address)
    {
    case PPU_CONTROL:
        break;
    case PPU_MASK:
        break;
    case PPU_STATUS:
        break;
    case PPU_OAM_ADDRESS:
        break;
    case PPU_OAM_DATA:
        break;
    case PPU_SCROLL:
        break;
    case PPU_ADDRESS:
        break;
    case PPU_DATA:
        break;
    default:
        break;
    }
}

uint32_t * PPU::frame_buffer()
{
    return m_frame_buffer;
}

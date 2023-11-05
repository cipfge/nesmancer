#include "ppu.hpp"
#include "cartridge.hpp"
#include "global.hpp"
#include <cstdlib>
#include <cstring>

// TODO: Generate or load from file
uint32_t PPU::m_palette[64] = {
    0X464646, 0X000154, 0X000070, 0X07006B,
    0X280048, 0X3C000E, 0X3E0000, 0X2C0000,
    0X0D0300, 0X001500, 0X001F00, 0X001F00,
    0X001420, 0X000000, 0X000000, 0X000000,
    0X9D9D9D, 0X0041B0, 0X1825D5, 0X4A0DCF,
    0X75009F, 0X900153, 0X920F00, 0X7B2800,
    0X514400, 0X205C00, 0X006900, 0X006916,
    0X005A6A, 0X000000, 0X000000, 0X000000,
    0XFEFFFF, 0X4896FF, 0X626DFF, 0X8E5BFF,
    0XD45EFF, 0XF160B4, 0XF36F5E, 0XDC8817,
    0XB2A400, 0X7FBD00, 0X53CA28, 0X38CA76,
    0X36BBCB, 0X2B2B2B, 0X000000, 0X000000,
    0XFEFFFF, 0XB0D2FF, 0XB6BBFF, 0XCBB4FF,
    0XEDBCFF, 0XF9BDE0, 0XFAC3BD, 0XF0CE9F,
    0XDFD990, 0XCAE393, 0XB8E9A6, 0XADE9C6,
    0XACE3E9, 0XA7A7A7, 0X000000, 0X000000
};

PPU::PPU(Cartridge* cartridge)
    : m_cartridge(cartridge)
{
}

PPU::~PPU()
{
}

void PPU::reset()
{
    memset(m_palette_ram, 0, sizeof(m_palette_ram));
    memset(m_oam, 0, sizeof(m_oam));

    m_oam_address = 0;
    m_control.value = 0;
    m_mask.value = 0;
    m_status.value = 0;
    m_vram_address.value = 0;
    m_vram_temp_address.value = 0;
    m_fine_x = 0;
    m_data_buffer = 0;
    m_cycle = 0;
    m_scanline = 0;
    m_frame_completed = false;
    m_offset_toggle = false;
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
    case PPU_STATUS:
        data = (m_status.value & 0xE0) | (m_data_buffer & 0x1F);
        m_status.vertical_blank = 0;
        m_offset_toggle = false;
        break;

    case PPU_OAM_DATA:
        m_data_buffer = m_oam[m_oam_address];
        break;

    case PPU_DATA:
        data = m_data_buffer;
        m_data_buffer = video_bus_read(m_vram_address.value);
        if (m_vram_address.value > 0x3EFF)
            data = m_data_buffer;
        m_vram_address.value += (m_control.increment_mode ? 32 : 1);
        break;

    default:
        break;
    }

    return data;
}

void PPU::write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case PPU_CONTROL:
        m_control.value = data;
        m_vram_temp_address.nametable = m_control.nametable;
        break;

    case PPU_MASK:
        m_mask.value = data;
        break;

    case PPU_OAM_ADDRESS:
        m_oam_address = data;
        break;

    case PPU_OAM_DATA:
        m_oam[m_oam_address++] = data;
        break;

    case PPU_SCROLL:
        if (m_offset_toggle)
        {
            m_vram_temp_address.coarse_y = (data >> 3) & 0x1F;
            m_vram_temp_address.fine_y = data & 7;
        }
        else
        {
            m_vram_temp_address.coarse_x = (data >> 3) & 0x1F;
            m_fine_x = data & 7;
        }
        m_offset_toggle = !m_offset_toggle;
        break;

    case PPU_ADDRESS:
        if (m_offset_toggle)
        {
            m_vram_temp_address.value = (m_vram_temp_address.value & 0xFF00) | data;
            m_vram_address = m_vram_temp_address;
        }
        else
        {
            m_vram_temp_address.value = (m_vram_temp_address.value & 0x00FF)
                                      | ((uint16_t)(data & 0x3F) << 8);
        }
        m_offset_toggle = !m_offset_toggle;
        break;

    case PPU_DATA:
        video_bus_write(m_vram_address.value, data);
        m_vram_address.value += (m_control.increment_mode ? 32 : 1);
        break;

    default:
        break;
    }
}

uint32_t* PPU::frame_buffer()
{
    return m_frame_buffer;
}

uint8_t PPU::video_bus_read(uint16_t address)
{
    uint8_t data = 0;
    if (address < 0x3F00)
        data = m_cartridge->ppu_read(address);
    else
    {
        uint16_t palette_address = (address - 0x3F00) & 0x1F;
        if (palette_address % 4 == 0)
            palette_address = 0;
        data = m_palette_ram[palette_address];
    }

    return data;
}

void PPU::video_bus_write(uint16_t address, uint8_t data)
{
    if (address < 0x3F00)
        m_cartridge->ppu_write(address, data);
    else
    {
        uint16_t palette_address = (address - 0x3F00) & 0x1F;
        if (palette_address > 0x0F && palette_address % 4 == 0)
            palette_address -= 0x10;
        m_palette_ram[palette_address] = data;
    }
}

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
    reset();
}

PPU::~PPU()
{
}

void PPU::reset()
{
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
    m_frame_odd = false;
    m_offset_toggle = false;
    m_cpu_nmi = false;

    m_oam_address = 0;
    m_sprite_count = 0;
    m_sprite_zero_hit_possible = false;

    m_bg_nametable = 0;
    m_bg_attribute = 0;
    m_bg_tile_low = 0;
    m_bg_tile_high = 0;

    memset(m_palette_ram, 0, sizeof(m_palette_ram));
    memset(m_oam, 0, sizeof(m_oam));
    memset(m_sprite_scanline, 0, sizeof(m_sprite_scanline));
    memset(m_frame_buffer, 0, sizeof(m_frame_buffer));
}

void PPU::frame_clear()
{
    m_frame_completed = false;
}

bool PPU::frame_completed()
{
    return m_frame_completed;
}

uint32_t* PPU::frame_buffer()
{
    return m_frame_buffer;
}

bool PPU::cpu_nmi() const
{
    return m_cpu_nmi && m_control.nmi;
}

void PPU::cpu_nmi_clear()
{
    m_cpu_nmi = false;
}

void PPU::tick()
{
    if (m_scanline < 240)
    {
        if (m_mask.show_background || m_mask.show_sprites)
            render_cycle();

        if (m_cycle < 256)
            render_pixel();
    }
    else if (m_scanline == 241 && m_cycle == 1)
    {
        m_status.vertical_blank = 1;
        m_cpu_nmi = true;
    }
    else if (m_scanline == 261)
    {
        if (m_mask.show_background || m_mask.show_sprites)
            render_cycle();

        if (m_cycle == 1)
        {
            m_status.vertical_blank = 0;
            m_status.sprite_zero_hit = 0;
            m_cpu_nmi = false;

            clear_sprite_shifters();
        }
        else if (m_cycle > 279 && m_cycle < 305)
        {
            if (m_mask.show_background || m_mask.show_sprites)
            {
                m_vram_address.fine_y = m_vram_temp_address.fine_y;
                m_vram_address.coarse_y = m_vram_temp_address.coarse_y;
                m_vram_address.nametable = (m_vram_address.nametable & 1) |
                                           (m_vram_temp_address.nametable & 2);
            }
        }
        else if (m_cycle == 340 && m_frame_odd && m_mask.show_background)
        {
            // Skip cycle 0 on odd frame when background is enabled
            m_cycle = 1;
            m_scanline = 0;
            m_frame_completed = true;
            m_frame_odd = !m_frame_odd;

            return;
        }
    }

    if ((m_scanline < 241 && m_cycle == 260) &&
        (m_mask.show_background || m_mask.show_sprites))
    {
        m_cartridge->ppu_scanline();
    }

    m_cycle++;
    if (m_cycle > 340)
    {
        m_cycle = 0;
        m_scanline++;
        if (m_scanline > 261)
        {
            m_scanline = 0;
            m_frame_completed = true;
            m_frame_odd = !m_frame_odd;
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
        m_cpu_nmi = false;
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
            m_vram_address.value = m_vram_temp_address.value;
        }
        else
        {
            m_vram_temp_address.value = (m_vram_temp_address.value & 0x00FF) |
                                        ((uint16_t)(data & 0x3F) << 8);
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

void PPU::render_cycle()
{
    if ((m_cycle > 1 && m_cycle < 258) || (m_cycle > 321 && m_cycle < 338))
    {
        if (m_mask.show_background)
        {
            m_bg_shifter.pattern_low <<= 1;
            m_bg_shifter.pattern_high <<= 1;
            m_bg_shifter.attribute_low <<= 1;
            m_bg_shifter.attribute_high <<= 1;
        }
    }

    if (m_cycle > 0 && (m_cycle < 256 || m_cycle > 320) && m_cycle < 337)
    {
        if (m_cycle < 256 && m_mask.show_sprites)
        {
            ObjectAttributeEntry* sprite = nullptr;
            for (int i = 0; i < m_sprite_count; i++)
            {
                sprite = m_sprite_scanline + i;
                if (sprite->x > 0)
                {
                    sprite->x--;
                }
                else
                {
                    m_sprite_shifter.pattern_low[i] <<= 1;
                    m_sprite_shifter.pattern_high[i] <<= 1;
                }
            }
        }

        switch ((m_cycle - 1) % 8)
        {
        case 0:
            m_bg_shifter.attribute_low = (m_bg_shifter.attribute_low & 0xFF00) |
                                         ((m_bg_attribute & 1) ? 0xFF : 0x00);
            m_bg_shifter.attribute_high = (m_bg_shifter.attribute_high & 0xFF00) |
                                          ((m_bg_attribute & 2) ? 0xFF : 0x00);
            m_bg_shifter.pattern_low = (m_bg_shifter.pattern_low & 0xFF00) | m_bg_tile_low;
            m_bg_shifter.pattern_high = (m_bg_shifter.pattern_high & 0xFF00) | m_bg_tile_high;

            m_bg_nametable = video_bus_read(0x2000 | (m_vram_address.value & 0x0FFF));
            break;

        case 2:
            m_bg_attribute = video_bus_read(0x23C0 |
                                            (m_vram_address.value & 0x0C00) |
                                            ((m_vram_address.value >> 4) & 0x38) |
                                            ((m_vram_address.value >> 2) & 0x7));
            if (m_vram_address.coarse_y & 2)
                m_bg_attribute >>= 4;
            if (m_vram_address.coarse_x & 2)
                m_bg_attribute >>= 4;

            m_bg_attribute &= 3;
            break;

        case 4:
        {
            m_bg_tile_low = video_bus_read(((uint16_t)m_control.background_table << 12) |
                                           (((uint16_t)m_bg_nametable) << 4) |
                                           m_vram_address.fine_y);
            break;
        }

        case 6:
            m_bg_tile_high = video_bus_read(((uint16_t)m_control.background_table << 12) |
                                            (((uint16_t)m_bg_nametable) << 4) |
                                            0x8 |
                                            m_vram_address.fine_y);
            break;

        case 7:
            scroll_x();
            break;

        default:
            break;
        }
    }
    else if (m_cycle == 256)
    {
        scroll_y();
    }
    else if (m_cycle == 257)
    {
        m_vram_address.coarse_x = m_vram_temp_address.coarse_x;
        m_vram_address.nametable = (m_vram_address.nametable & 2) |
                                   (m_vram_temp_address.nametable & 1);

        evaluate_sprites();
    }
    else if (m_cycle == 337 || m_cycle == 339)
    {
        m_bg_nametable = video_bus_read(0x2000 | (m_vram_address.value & 0x0FFF));
    }
}

void PPU::render_pixel()
{
    uint8_t bg_pixel = 0;
    uint8_t bg_palette = 0;
    uint8_t fg_pixel = 0;
    uint8_t fg_palette = 0;
    uint8_t fg_priority  = 0;
    uint8_t pixel = 0;
    uint8_t palette = 0;

    if (m_mask.show_background)
    {
        uint8_t bit = 15 - m_fine_x;
        bg_pixel = ((m_bg_shifter.pattern_low >> bit) & 1) | (((m_bg_shifter.pattern_high >> bit) & 1) << 1);
        bg_palette = ((m_bg_shifter.attribute_low >> bit) & 1) | (((m_bg_shifter.attribute_high >> bit) & 1) << 1);
    }

    if (m_cycle < 8 && !m_mask.background_left)
    {
        bg_pixel = 0;
        bg_palette = 0;
    }

    if (m_mask.show_sprites)
    {
        ObjectAttributeEntry* sprite = nullptr;
        for (int i = 0; i < m_sprite_count; i++)
        {
            sprite = m_sprite_scanline + i;
            if (sprite->x == 0)
            {
                uint8_t low = (m_sprite_shifter.pattern_low[i] >> 7) & 1;
                uint8_t high = (m_sprite_shifter.pattern_high[i] >> 7) & 1;
                fg_palette = (high << 1) | low;
                fg_pixel = (sprite->attribute & 0x3) + 4;
                fg_priority = (sprite->attribute >> 5) & 1;
            }

            if (i == 0)
                set_sprite_zero_hit(fg_palette, bg_palette);

            if (fg_palette != 0)
                break;
        }
    }

    if (m_cycle < 8 && !m_mask.sprite_left)
    {
        fg_pixel = 0;
        fg_palette = 0;
    }

    if (bg_pixel == 0 && fg_pixel == 0)
    {
        pixel = 0;
        palette = 0;
    }
    else if (fg_pixel > 0 && (fg_priority == 0 || bg_pixel == 0))
    {
        pixel = fg_pixel;
        palette = fg_palette;
    }
    else
    {
        pixel = bg_pixel;
        palette = bg_palette;
    }

    uint32_t color = read_color_from_palette(palette, pixel);
    m_frame_buffer[m_scanline * EMU_SCREEN_WIDTH + m_cycle] = color;
}

void PPU::scroll_x()
{
    if (!m_mask.show_background && !m_mask.show_sprites)
        return;

    m_vram_address.coarse_x++;
    if (m_vram_address.coarse_x == 0)
        m_vram_address.nametable ^= 1;
}

void PPU::scroll_y()
{
    if (!m_mask.show_background && !m_mask.show_sprites)
        return;

    m_vram_address.fine_y++;
    if (m_vram_address.fine_y == 0)
    {
        if (m_vram_address.coarse_y == 30)
        {
            m_vram_address.coarse_y = 0;
            m_vram_address.nametable ^= 2;
        }
    }
}

void PPU::clear_sprite_shifters()
{
    memset(m_sprite_shifter.pattern_low, 0, sizeof(m_sprite_shifter.pattern_low));
    memset(m_sprite_shifter.pattern_high, 0, sizeof(m_sprite_shifter.pattern_high));
}

uint8_t PPU::reverse_byte(uint8_t byte)
{
    uint8_t value = 0;

    value |= (byte & 0x80) >> 7;
    value |= (byte & 0x40) >> 5;
    value |= (byte & 0x20) >> 3;
    value |= (byte & 0x10) >> 1;
    value |= (byte & 0x08) << 1;
    value |= (byte & 0x04) << 3;
    value |= (byte & 0x02) << 5;
    value |= (byte & 0x01) << 7;

    return value;
}

void PPU::evaluate_sprites()
{
    if (m_scanline == 261)
        return;

    memset(m_sprite_scanline, 0xFF, 8 * sizeof(ObjectAttributeEntry));
    m_sprite_count = 0;

    m_status.sprite_overflow = 0;
    m_sprite_zero_hit_possible = false;

    ObjectAttributeEntry* sprite = nullptr;
    int sprite_row = 0;
    int sprite_height = m_control.sprite_size ? 16 : 8;

    for (int i = 0; i < 256; i += 4)
    {
        sprite = (ObjectAttributeEntry*)(m_oam + i);
        sprite_row = m_scanline - sprite->y;

        if (m_sprite_count < 9 && sprite_row >= 0 && sprite_row < sprite_height)
        {
            if (m_sprite_count == 8)
            {
                m_status.sprite_overflow = 1;
                break;
            }

            if (i == 0)
                m_sprite_zero_hit_possible = true;

            if (sprite->attribute & SPRITE_ATTR_FLIP_VERTICAL)
                sprite_row = sprite_height - 1 - sprite_row;

            uint8_t pattern_table = m_control.sprite_table;
            uint8_t tile_index = sprite->id;

            if (sprite_height == 16)
            {
                pattern_table = sprite->id & 1;
                tile_index = sprite->id & 0xFE;
                if (sprite_row > 7)
                    tile_index++;
                sprite_row &= 0x07;
            }

            uint16_t sprite_address = (pattern_table << 12) |
                                      (tile_index * 16) |
                                      sprite_row;

            uint8_t sprite_data_low = video_bus_read(sprite_address);
            uint8_t sprite_data_high = video_bus_read(sprite_address + 8);

            if (sprite->attribute & SPRITE_ATTR_FLIP_HORIZONTAL)
            {
                sprite_data_low = reverse_byte(sprite_data_low);
                sprite_data_high = reverse_byte(sprite_data_high);
            }

            m_sprite_shifter.pattern_low[m_sprite_count] = sprite_data_low;
            m_sprite_shifter.pattern_high[m_sprite_count] = sprite_data_high;

            memcpy(m_sprite_scanline + m_sprite_count, m_oam + i, sizeof(m_sprite_scanline[0]));
            m_sprite_count++;
        }
    }
}

void PPU::set_sprite_zero_hit(uint8_t fg_palette, uint8_t bg_palette)
{
    if (m_sprite_zero_hit_possible && fg_palette > 0 && bg_palette > 0 &&
        (m_cycle > 7 || (m_mask.background_left && m_mask.sprite_left)) &&
        m_cycle > 1 && m_cycle != 255)
    {
        m_status.sprite_zero_hit = 1;
    }
}

uint32_t PPU::read_color_from_palette(uint8_t palette, uint8_t pixel)
{
    uint16_t address = pixel * 4 + palette;
    if (address > 0x0F && address % 4 == 0)
        address -= 0x10;

    return m_palette[m_palette_ram[address]];
}

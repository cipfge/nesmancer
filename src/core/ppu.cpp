#include "ppu.hpp"
#include "cartridge.hpp"
#include <cstring>

uint32_t PPU::m_default_palette[64] = {
    0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
    0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
    0xADADAD, 0x155FD9, 0x4240FF, 0x7527FE, 0xA01ACC, 0xB71E7B, 0xB53120, 0x994E00,
    0x6B6D00, 0x388700, 0x0C9300, 0x008F32, 0x007C8D, 0x000000, 0x000000, 0x000000,
    0xFFFEFF, 0x64B0FF, 0x9290FF, 0xC676FF, 0xF36AFF, 0xFE6ECC, 0xFE8170, 0xEA9E22,
    0xBCBE00, 0x88D800, 0x5CE430, 0x45E082, 0x48CDDE, 0x4F4F4F, 0x000000, 0x000000,
    0xFFFEFF, 0xC0DFFF, 0xD3D2FF, 0xE8C8FF, 0xFBC2FF, 0xFEC4EA, 0xFECCC5, 0xF7D8A5,
    0xE4E594, 0xCFEF96, 0xBDF4AB, 0xB3F3CC, 0xB5EBF2, 0xB8B8B8, 0x000000, 0x000000
};

PPU::PPU(Cartridge& cartridge)
    : m_cartridge(cartridge)
{
    load_default_palette();
    reset();
}

void PPU::reset()
{
    m_control.value = 0;
    m_mask.value = 0;
    m_status.value = 0;
    m_vram_address .value = 0;
    m_tram_address.value = 0;
    m_fine_x = 0;
    m_cycle = 0;
    m_scanline = 0;
    m_data_buffer = 0;
    m_offset = false;
    m_nmi = false;

    m_bg_tile.nametable = 0;
    m_bg_tile.attribute = 0;
    m_bg_tile.byte_low = 0;
    m_bg_tile.byte_high = 0;

    m_bg_shifter.pattern_low = 0;
    m_bg_shifter.pattern_high = 0;
    m_bg_shifter.attribute_low = 0;
    m_bg_shifter.attribute_high = 0;

    m_oam_address = 0;
    m_sprite_count = 0;
    m_sprite_zero_hit_possible = false;

    m_frame_rendered = false;
    m_frame_odd = false;

    memset(m_palette_ram, 0xFF, sizeof(m_palette_ram));
    memset(m_oam, 0xFF, sizeof(m_oam));
    memset(m_oam_scanline, 0xFF, sizeof(m_oam_scanline));
    memset(m_frame_buffer, 0xFF, sizeof(m_frame_buffer));
}

void PPU::tick()
{
    if (m_scanline < 240)
    {
        if (is_rendering())
            render_cycle();

        if (m_cycle < 256)
            render_pixel();
    }
    else if (m_scanline == 241 && m_cycle == 1)
    {
        m_status.vertical_blank = 1;
        m_nmi = true;
    }
    else if (m_scanline == 261)
    {
        if (is_rendering())
            render_cycle();

        if (m_cycle == 1)
        {
            m_status.vertical_blank = 0;
            m_status.sprite_zero_hit = 0;
            m_nmi = false;
            clear_sprite_shifter();
        }
        else if (m_cycle > 279 && m_cycle < 305)
        {
            if (is_rendering())
                address_transfer_y();
        }
        else if (m_cycle == 340 && m_frame_odd && m_mask.render_background)
        {
            m_cycle = 1;
            m_scanline = 0;
            m_frame_rendered = true;
            m_frame_odd = !m_frame_odd;
            return;
        }
    }

    if (is_rendering() && (m_scanline < 241 && m_cycle == 260))
        m_cartridge.scanline();

    m_cycle++;
    if (m_cycle > 340)
    {
        m_cycle = 0;
        m_scanline++;
        if (m_scanline > 261)
        {
            m_scanline = 0;
            m_frame_rendered = true;
            m_frame_odd = !m_frame_odd;
        }
    }
}

uint8_t PPU::read(uint16_t address)
{
    uint8_t data = 0;
    uint16_t reg = address & 0x7;

    switch (reg)
    {
    case PPU_STATUS:
        data = (m_status.value & 0xE0) | (m_data_buffer & 0x1F);
        m_status.vertical_blank = 0;
        m_nmi = false;
        m_offset = false;
        break;

    case PPU_OAM_DATA:
        data = m_oam[m_oam_address];
        break;

    case PPU_DATA:
        data = m_data_buffer;
        m_data_buffer = video_bus_read(m_vram_address.value);
        if (m_vram_address.value > 0x3EFF)
            data = m_data_buffer;
        m_vram_address.value += (m_control.address_increment ? 32 : 1);
        break;

    default:
        break;
    }

    return data;
}

void PPU::write(uint16_t address, uint8_t data)
{
    uint16_t reg = address & 0x7;

    switch(reg)
    {
    case PPU_CONTROL:
        m_control.value = data;
        m_tram_address.nametable = m_control.nametable;
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
        if (!m_offset)
        {
            m_tram_address.coarse_x = (data >> 3) & 0x1F;
            m_fine_x = data & 0x7;
        }
        else
        {
            m_tram_address.coarse_y = (data >> 3) & 0x1F;;
            m_tram_address.fine_y = data & 0x7;
        }
        m_offset = !m_offset;
        break;

    case PPU_ADDRESS:
        if (!m_offset)
        {
            m_tram_address.value = (m_tram_address.value & 0x00FF) |
                                   ((uint16_t)(data & 0x3F) << 8);
        }
        else
        {
            m_tram_address.value = (m_tram_address.value & 0xFF00) | data;
            m_vram_address = m_tram_address;
        }
        m_offset = !m_offset;
        break;

    case PPU_DATA:
        video_bus_write(m_vram_address.value, data);
        m_vram_address.value += (m_control.address_increment ? 32 : 1);
        break;

    default:
        break;
    }
}

void PPU::set_palette(const uint32_t* palette)
{
    for (int i = 0; i < 64; i++)
        m_palette[i] = palette[i];
}

uint16_t PPU::nametable_mirror(uint16_t address)
{
    switch (m_cartridge.mirroring_mode())
    {
    case MirroringMode::Horizontal:
        return ((address / 2) & 0x400) + (address % 0x400);
    case MirroringMode::Vertical:
        return address % 0x800;
    default:
        return address - 0x2000;
    }
}

uint8_t PPU::video_bus_read(uint16_t address)
{
    uint8_t data = 0;
    if (address < 0x2000)
        data = m_cartridge.ppu_read(address);
    else if (address < 0x3F00)
        data = m_video_ram[nametable_mirror(address)];
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
    if (address < 0x2000)
        m_cartridge.ppu_write(address, data);
    else if (address < 0x3F00)
        m_video_ram[nametable_mirror(address)] = data;
    else
    {
        uint16_t palette_address = (address - 0x3F00) & 0x1F;
        if (palette_address > 0x0F && palette_address % 4 == 0)
            palette_address -= 0x10;
        m_palette_ram[palette_address] = data;
    }
}

inline uint32_t PPU::read_color_from_palette(uint8_t pixel, uint8_t palette)
{
    uint16_t address = palette * 4 + pixel;
    if (address > 0x0F && address % 4 == 0)
        address -= 0x10;

    return m_palette[m_palette_ram[address]];
}

inline bool PPU::is_rendering()
{
    return (m_mask.render_background || m_mask.render_sprites);
}

inline void PPU::address_transfer_x()
{
    m_vram_address.coarse_x = m_tram_address.coarse_x;
    m_vram_address.nametable = (m_vram_address.nametable & 2) |
                               (m_tram_address.nametable & 1);
}

inline void PPU::address_transfer_y()
{
    m_vram_address.coarse_y = m_tram_address.coarse_y;
    m_vram_address.fine_y = m_tram_address.fine_y;
    m_vram_address.nametable = (m_vram_address.nametable & 1) |
                               (m_tram_address.nametable & 2);
}

inline void PPU::scroll_horizontal()
{
    m_vram_address.coarse_x++;
    if (m_vram_address.coarse_x == 0)
        m_vram_address.nametable ^= 1;
}

inline void PPU::scroll_vertical()
{
    m_vram_address.fine_y++;
    if (m_vram_address.fine_y == 0)
    {
        m_vram_address.coarse_y++;
        if (m_vram_address.coarse_y == 30)
        {
            m_vram_address.coarse_y = 0;
            m_vram_address.nametable ^= 2;
        }
    }
}

inline void PPU::load_background_shifter()
{
    m_bg_shifter.pattern_low = (m_bg_shifter.pattern_low & 0xFF00) | m_bg_tile.byte_low;
    m_bg_shifter.pattern_high = (m_bg_shifter.pattern_high & 0xFF00) | m_bg_tile.byte_high;

    m_bg_shifter.attribute_low = (m_bg_shifter.attribute_low & 0xFF00) |
                                 ((m_bg_tile.attribute & 1) ? 0xFF : 0);
    m_bg_shifter.attribute_high = (m_bg_shifter.attribute_high & 0xFF00) |
                                  ((m_bg_tile.attribute & 2) ? 0xFF : 0);
}

inline void PPU::update_background_shifter()
{
    m_bg_shifter.pattern_low <<= 1;
    m_bg_shifter.pattern_high <<= 1;
    m_bg_shifter.attribute_low <<= 1;
    m_bg_shifter.attribute_high <<= 1;
}

inline void PPU::update_sprite_shifter()
{
    Sprite* sprite = nullptr;
    for (int i = 0; i < m_sprite_count; i++)
    {
        sprite = m_oam_scanline + i;
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

inline void PPU::clear_sprite_shifter()
{
    memset(m_sprite_shifter.pattern_low, 0, 8 * sizeof(m_sprite_shifter.pattern_low[0]));
    memset(m_sprite_shifter.pattern_high, 0, 8 * sizeof(m_sprite_shifter.pattern_high[0]));
}

void PPU::update_sprites()
{
    if (m_scanline == 261)
        return;

    memset(m_oam_scanline, 0xFF, 8 * sizeof(Sprite));
    m_sprite_count = 0;

    m_status.sprite_overflow = 0;
    m_sprite_zero_hit_possible = false;

    Sprite* sprite = nullptr;
    int sprite_row = 0;
    int sprite_height = m_control.sprite_size ? 16 : 8;

    for (int i = 0; i < 256; i += 4)
    {
        sprite = (Sprite*)(m_oam + i);
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

            memcpy(m_oam_scanline + m_sprite_count,
                   m_oam + i,
                   sizeof(m_oam_scanline[0]));

            m_sprite_count++;
        }
    }
}

void PPU::sprite_zero_hit(uint8_t spr_pixel, uint8_t bg_pixel)
{
    if (m_sprite_zero_hit_possible && spr_pixel > 0 && bg_pixel > 0 &&
        (m_cycle > 7 || (m_mask.background_left && m_mask.sprites_left)) &&
        m_cycle > 1 && m_cycle != 255)
    {
        m_status.sprite_zero_hit = 1;
    }
}

void PPU::render_cycle()
{
    if ((m_cycle > 1 && m_cycle < 258) || (m_cycle > 321 && m_cycle < 338))
    {
        if (m_mask.render_background)
            update_background_shifter();
    }

    if (m_cycle > 0 && (m_cycle < 256 || m_cycle > 320) && m_cycle < 337)
    {
        if (m_mask.render_sprites && m_cycle < 256)
            update_sprite_shifter();

        switch ((m_cycle - 1) % 8)
        {
        case 0:
            load_background_shifter();
            m_bg_tile.nametable = video_bus_read(0x2000 | (m_vram_address.value & 0x0FFF));
            break;

        case 2:
            m_bg_tile.attribute = video_bus_read(0x23C0 |
                                                 (m_vram_address.value & 0x0C00) |
                                                 ((m_vram_address.value >> 4) & 0x38) |
                                                 ((m_vram_address.value >> 2) & 0x7));
            if (m_vram_address.coarse_y & 2)
                m_bg_tile.attribute >>= 4;
            if (m_vram_address.coarse_x & 2)
                m_bg_tile.attribute >>= 2;
            m_bg_tile.attribute &= 3;
            break;

        case 4:
            m_bg_tile.byte_low = video_bus_read(((uint16_t)m_control.background_table << 12) |
                                                (((uint16_t)m_bg_tile.nametable) << 4) |
                                                m_vram_address.fine_y);
            break;

        case 6:
            m_bg_tile.byte_high = video_bus_read(((uint16_t)m_control.background_table << 12) |
                                                 (((uint16_t)m_bg_tile.nametable) << 4) |
                                                 m_vram_address.fine_y |
                                                 0x8);
            break;

        case 7:
            scroll_horizontal();
            break;

        default:
            break;
        }
    }
    else if (m_cycle == 256)
    {
        scroll_vertical();
    }
    else if (m_cycle == 257)
    {
        address_transfer_x();
        update_sprites();
    }
    else if (m_cycle == 337 || m_cycle == 339)
    {
        m_bg_tile.nametable = video_bus_read(0x2000 | (m_vram_address.value & 0x0FFF));
    }
}

void PPU::render_pixel()
{
    uint8_t pixel = 0;
    uint8_t palette = 0;
    uint8_t bg_pixel = 0;
    uint8_t bg_palette = 0;
    uint8_t spr_pixel = 0;
    uint8_t spr_palette = 0;
    uint8_t spr_priority = 0;

    if (m_mask.render_background)
    {
        uint8_t bit = 15 - m_fine_x;
        bg_pixel = ((m_bg_shifter.pattern_low >> bit) & 1) |
                   (((m_bg_shifter.pattern_high >> bit) & 1) << 1);
        bg_palette = ((m_bg_shifter.attribute_low >> bit) & 1) |
                     (((m_bg_shifter.attribute_high >> bit) & 1) << 1);
    }

    if (m_cycle < 8 && !m_mask.background_left)
    {
        bg_pixel = 0;
        bg_palette = 0;
    }

    if (m_mask.render_sprites)
    {
        Sprite* sprite = nullptr;
        for (int i = 0; i < m_sprite_count; i++)
        {
            sprite = m_oam_scanline + i;
            if (sprite->x == 0)
            {
                uint8_t low = (m_sprite_shifter.pattern_low[i] >> 7) & 1;
                uint8_t high = (m_sprite_shifter.pattern_high[i] >> 7) & 1;
                spr_pixel = (high << 1) | low;
                spr_palette = (sprite->attribute & 0x3) + 4;
                spr_priority = (sprite->attribute >> 5) & 1;
            }

            if (i == 0)
                sprite_zero_hit(spr_pixel, bg_pixel);

            if (spr_pixel != 0)
                break;
        }
    }

    if (m_cycle < 8 && !m_mask.sprites_left)
    {
        spr_pixel = 0;
        spr_palette = 0;
    }

    if (spr_pixel == 0 && bg_pixel == 0)
    {
        pixel = 0;
        palette = 0;
    }
    else if (spr_pixel > 0 && (spr_priority == 0 || bg_pixel == 0))
    {
        pixel = spr_pixel;
        palette = spr_palette;
    }
    else
    {
        pixel = bg_pixel;
        palette = bg_palette;
    }

    uint32_t color = read_color_from_palette(pixel, palette);
    m_frame_buffer[m_scanline * ScreenWidth + m_cycle] = color;
}

void PPU::load_default_palette()
{
    for (int i = 0; i < 64; i++)
        m_palette[i] = m_default_palette[i];
}
#pragma once

#include <cstdint>
#include <array>

class Cartridge;

class PPU
{
public:
    enum Registers
    {
        PPU_CONTROL,
        PPU_MASK,
        PPU_STATUS,
        PPU_OAM_ADDRESS,
        PPU_OAM_DATA,
        PPU_SCROLL,
        PPU_ADDRESS,
        PPU_DATA
    };

public:
    PPU(Cartridge& cartridge);
    ~PPU();

    static constexpr uint16_t ScreenWidth = 256;
    static constexpr uint16_t ScreenHeight = 240;

    void reset();
    void tick();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

    uint16_t cycle() const { return m_cycle; }
    uint16_t scanline() const { return m_scanline; }
    void frame_start() { m_frame_rendered = false; }
    bool frame_rendered() const { return m_frame_rendered; }
    uint32_t* frame_buffer() { return m_frame_buffer; }
    bool nmi() const { return (m_control.nmi_enabled && m_nmi); }
    void nmi_clear() { m_nmi = false; }

    uint8_t control() const { return m_control.value; }
    uint8_t mask() const { return m_mask.value; }
    uint8_t status() const { return m_status.value; }
    const uint8_t* oam() const { return m_oam; }

private:
    union Control
    {
        struct
        {
            uint8_t nametable: 2;
            uint8_t address_increment: 1;
            uint8_t sprite_table: 1;
            uint8_t background_table: 1;
            uint8_t sprite_size: 1;
            uint8_t select_mode: 1;
            uint8_t nmi_enabled: 1;
        };
        uint8_t value = 0;
    };

    union Mask
    {
        struct
        {
            uint8_t greyscale: 1;
            uint8_t background_left: 1;
            uint8_t sprites_left: 1;
            uint8_t render_background: 1;
            uint8_t render_sprites: 1;
            uint8_t enhance_red: 1;
            uint8_t enhance_green: 1;
            uint8_t enhance_blue: 1;
        };
        uint8_t value = 0;
    };

    union Status
    {
        struct
        {
            uint8_t unused: 5;
            uint8_t sprite_overflow: 1;
            uint8_t sprite_zero_hit: 1;
            uint8_t vertical_blank: 1;
        };
        uint8_t value = 0;
    };

    union Address
    {
        struct
        {
            uint16_t coarse_x: 5;
            uint16_t coarse_y: 5;
            uint16_t nametable: 2;
            uint16_t fine_y: 3;
            uint16_t unused: 1;
        };
        uint16_t value = 0;
    };

    struct BackgroundTile
    {
        uint8_t nametable = 0;
        uint8_t attribute = 0;
        uint8_t byte_low = 0;
        uint8_t byte_high = 0;
    };

    struct BackgroundShifter
    {
        uint16_t pattern_low = 0;
        uint16_t pattern_high = 0;
        uint16_t attribute_low = 0;
        uint16_t attribute_high = 0;
    };

    enum SpriteAttribute
    {
        SPRITE_ATTR_PRIORITY = (1 << 5),
        SPRITE_ATTR_FLIP_HORIZONTAL = (1 << 6),
        SPRITE_ATTR_FLIP_VERTICAL = (1 << 7)
    };

    struct Sprite
    {
        uint8_t y = 0;
        uint8_t id = 0;
        uint8_t attribute = 0;
        uint8_t x = 0;
    };

    struct SpriteShifter
    {
        uint8_t pattern_low[8];
        uint8_t pattern_high[8];
    };

    static uint32_t m_palette[64];

    Cartridge& m_cartridge;
    Control m_control;
    Mask m_mask;
    Status m_status;
    Address m_vram_address;
    Address m_tram_address;
    uint8_t m_palette_ram[32];
    uint8_t m_fine_x = 0;
    uint16_t m_cycle = 0;
    uint16_t m_scanline = 0;
    uint8_t m_data_buffer = 0;
    bool m_offset = false;
    bool m_nmi = false;

    BackgroundTile m_bg_tile;
    BackgroundShifter m_bg_shifter;

    std::array<uint8_t, 0x800> m_video_ram;

    uint8_t m_oam[256];
    uint8_t m_oam_address = 0;
    Sprite m_oam_scanline[8];
    uint8_t m_sprite_count = 0;
    bool m_sprite_zero_hit_possible = false;
    SpriteShifter m_sprite_shifter;

    uint32_t m_frame_buffer[ScreenWidth * ScreenHeight];
    bool m_frame_rendered = false;
    bool m_frame_odd = false;

    uint16_t nametable_mirror(uint16_t address);
    uint8_t video_bus_read(uint16_t address);
    void video_bus_write(uint16_t address, uint8_t data);
    uint32_t read_color_from_palette(uint8_t pixel, uint8_t palette);
    bool is_rendering();
    void address_transfer_x();
    void address_transfer_y();
    void scroll_horizontal();
    void scroll_vertical();
    void load_background_shifter();
    void update_background_shifter();
    void update_sprite_shifter();
    uint8_t reverse_byte(uint8_t byte);
    void clear_sprite_shifter();
    void update_sprites();
    void sprite_zero_hit(uint8_t spr_pixel, uint8_t bg_pixel);
    void render_cycle();
    void render_pixel();
};

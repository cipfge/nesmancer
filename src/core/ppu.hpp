#pragma once

#include "types.hpp"
#include "global.hpp"

class Cartridge;

class PPU
{
public:
    enum Registers
    {
        PPU_CONTROL = 0,
        PPU_MASK,
        PPU_STATUS,
        PPU_OAM_ADDRESS,
        PPU_OAM_DATA,
        PPU_SCROLL,
        PPU_ADDRESS,
        PPU_DATA
    };

    union Control
    {
        uint8_t value = 0;
        struct
        {
            uint8_t nametable: 2;
            uint8_t increment_mode: 1;
            uint8_t sprite_table: 1;
            uint8_t background_table: 1;
            uint8_t sprite_size: 1;
            uint8_t mode: 1;
            uint8_t nmi: 1;
        };
    };

    union Mask
    {
        uint8_t value = 0;
        struct
        {
            uint8_t grayscale: 1;
            uint8_t background_left: 1;
            uint8_t sprite_left: 1;
            uint8_t show_background: 1;
            uint8_t show_sprites: 1;
            uint8_t red: 1;
            uint8_t green: 1;
            uint8_t blue: 1;
        };
    };

    union Status
    {
        uint8_t value = 0;
        struct
        {
            uint8_t unused: 5;
            uint8_t sprite_overflow: 1;
            uint8_t sprite_zero_hit: 1;
            uint8_t vertical_blank: 1;
        };
    };

    union LoopyAddress
    {
        uint16_t value = 0;
        struct
        {
            uint16_t coarse_x: 5;
            uint16_t coarse_y: 5;
            uint16_t nametable: 2;
            uint16_t fine_y:3;
        };
    };

    struct ObjectAttributeEntry
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

    struct BackgroundShifter
    {
        uint16_t pattern_low = 0;
        uint16_t pattern_high = 0;
        uint16_t attribute_low = 0;
        uint16_t attribute_high = 0;
    };

public:
    PPU(Cartridge* cartridge);
    ~PPU();

    void reset();
    void frame_clear();
    bool frame_completed();
    bool cpu_nmi() const;
    void cpu_nmi_clear();
    void tick();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

    uint32_t* frame_buffer();

private:
    static uint32_t m_palette[64];

    Cartridge* m_cartridge = nullptr;
    Control m_control;
    Mask m_mask;
    Status m_status;
    LoopyAddress m_vram_address;
    LoopyAddress m_vram_temp_address;

    uint8_t m_palette_ram[32];
    uint8_t m_fine_x = 0;
    uint8_t m_data_buffer = 0;
    uint16_t m_cycle = 0;
    uint16_t m_scanline = 0;
    bool m_frame_completed = false;
    bool m_frame_odd = false;
    bool m_offset_toggle = false;
    bool m_cpu_nmi = false;

    uint8_t m_oam[256];
    uint8_t m_oam_address = 0;
    ObjectAttributeEntry m_sprite_scanline[8];
    uint8_t m_sprite_count = 0;
    SpriteShifter m_sprite_shifter;

    uint8_t m_bg_nametable = 0;
    uint8_t m_bg_attribute = 0;
    uint8_t m_bg_tile_low = 0;
    uint8_t m_bg_tile_high = 0;
    BackgroundShifter m_bg_shifter;

    uint32_t m_frame_buffer[EMU_SCREEN_WIDTH * EMU_SCREEN_HEIGHT];

    void render_cycle();
    void render_pixel();
    uint8_t video_bus_read(uint16_t address);
    void video_bus_write(uint16_t address, uint8_t data);
    uint32_t read_color_from_palette(int nr, int index);
};

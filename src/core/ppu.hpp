#pragma once

#include "types.hpp"

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

public:
    PPU();
    ~PPU();

    void reset();
    void frame_clear();
    bool frame_completed();
    void tick();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    uint8_t m_oam[256];
    uint8_t m_oam_address = 0;
    Control m_control;
    Mask m_mask;
    Status m_status;
    LoopyAddress m_vram_address;
    LoopyAddress m_vram_temp_address;
    uint8_t m_data_buffer = 0;
    bool m_frame_completed = false;
};

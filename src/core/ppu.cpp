#include "ppu.hpp"
#include "global.hpp"

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
    m_frame_completed = false;
}

void PPU::frame_clear()
{
    m_frame_completed = false;
}

bool PPU::frame_completed()
{
    return true;
    // TODO: return m_frame_completed;
}

void PPU::tick()
{
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

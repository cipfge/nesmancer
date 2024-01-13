#pragma once

#include "mapper.hpp"

class MMC1 : public Mapper
{
public:
    enum NametableMirrorMode
    {
        NT_ONE_SCREEN_LO = 0,
        NT_ONE_SCREEN_HI,
        NT_VERTICAL,
        NT_HORIZONTAL
    };

public:
    MMC1(NesFile& nes_file): Mapper(nes_file) {}
    virtual ~MMC1() {}

    std::string name() const override { return "MMC1"; }
    uint32_t read(uint16_t address) override;
    uint32_t write(uint16_t address, uint8_t data) override;
    bool irq() override { return false; }
    void irq_clear() override {}
    void scanline() override {}

private:
    uint8_t m_character_bank0 = 0;
    uint8_t m_character_bank1 = 0;
    uint8_t m_program_bank = 0;
    uint8_t m_shift_register = 0;
    uint8_t m_shift_count = 0;
    uint8_t m_control_register = 0;

    uint32_t map_address(uint16_t address) const;
};

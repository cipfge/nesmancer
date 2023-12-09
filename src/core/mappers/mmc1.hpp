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
    MMC1(uint8_t prg_bank_count,
         uint8_t chr_bank_count,
         MirroringMode mirroring_mode);

    virtual ~MMC1();

    virtual uint32_t read(uint16_t address) override;
    virtual uint32_t write(uint16_t address, uint8_t data) override;
    virtual bool irq() override { return false; }
    virtual void irq_clear() override {}
    virtual void scanline() override {}
    virtual std::string name() const override { return "MMC1"; }

private:
    uint8_t m_chr_bank0 = 0;
    uint8_t m_chr_bank1 = 0;
    uint8_t m_prg_bank = 0;
    uint8_t m_shift_register = 0;
    uint8_t m_shift_count = 0;
    uint8_t m_control_register = 0;

    uint32_t map_address(uint16_t address) const;
};

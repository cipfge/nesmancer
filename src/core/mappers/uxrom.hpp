#pragma once

#include "mapper.hpp"

class UxROM : public Mapper
{
public:
    UxROM(uint8_t prg_bank_count,
          uint8_t chr_bank_count,
          MirroringMode mirroring_mode);

    virtual ~UxROM();

    virtual uint32_t read(uint16_t address) override;
    virtual uint32_t write(uint16_t address, uint8_t data) override;
    virtual bool irq() override { return false; }
    virtual void irq_clear() override {}
    virtual void scanline() override {}
    virtual std::string name() const override { return "UxROM"; }

private:
    uint8_t m_prg_bank = 0;

    uint32_t map_address(uint16_t address);
};

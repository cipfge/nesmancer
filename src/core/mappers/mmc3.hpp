#pragma once

#include "mapper.hpp"

class MMC3 : public Mapper
{
public:
    MMC3(uint8_t prg_bank_count,
         uint8_t chr_bank_count,
         MirroringMode mirroring_mode);

    virtual ~MMC3();

    uint32_t read(uint16_t address) override;
    uint32_t write(uint16_t address, uint8_t data) override;
    bool irq() override { return m_irq; }
    void irq_clear() override { m_irq = false; }
    void scanline() override;
    std::string name() const override { return "MMC3"; }

private:
    uint8_t m_bank_registers[8];
    uint8_t m_bank_target = 0;
    uint8_t m_chr_bank[8];
    uint8_t m_prg_bank[4];
    bool m_chr_inversion = false;
    bool m_prg_bank_mode = false;
    bool m_irq_enabled = false;
    uint8_t m_irq_counter = 0;
    uint8_t m_irq_reload = 0;
    bool m_irq = false;

    uint32_t map_address(uint16_t address) const;
};

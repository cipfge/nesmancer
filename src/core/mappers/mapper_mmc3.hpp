#pragma once

#include "mapper.hpp"

class Mapper_MMC3 : public Mapper
{
public:
    Mapper_MMC3(NesRom& rom);
    virtual ~Mapper_MMC3() {}

protected:
    void cpu_write(uint16_t address, uint8_t data) override;
    void ppu_write(uint16_t address, uint8_t data) override;
    bool irq() override { return m_irq; }
    void irq_clear() override { m_irq = false; }
    void scanline() override;

private:
    uint8_t m_tregister = 0;
    std::array<uint8_t, 8> m_registers = {};
    uint8_t m_irq_time = 0;
    uint8_t m_irq_count = 0;
    bool m_irq_enabled = false;
    bool m_irq = false;
    bool m_horizontal_mirroring = true;

    void configure();
};

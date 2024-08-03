#pragma once

#include "mapper.hpp"

class Mapper_MMC1 : public Mapper
{
public:
    Mapper_MMC1(NesRom& rom);
    virtual ~Mapper_MMC1() {}

protected:
    void cpu_write(uint16_t address, uint8_t data) override;
    void ppu_write(uint16_t address, uint8_t data) override;

private:
    uint8_t m_shift_register = 0;
    uint8_t m_count = 0;
    std::array<uint8_t, 4> m_registers = {};

    void configure();
};

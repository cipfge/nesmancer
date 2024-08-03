#pragma once

#include "mapper.hpp"

class Mapper_NROM : public Mapper
{
public:
    Mapper_NROM(NesRom& rom);
    virtual ~Mapper_NROM() {}

protected:
    void cpu_write(uint16_t address, uint8_t data) override;
    void ppu_write(uint16_t address, uint8_t data) override;
};

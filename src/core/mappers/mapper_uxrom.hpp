#pragma once

#include "mapper.hpp"

class Mapper_UxROM : public Mapper
{
public:
    Mapper_UxROM(NesRom& rom);
    virtual ~Mapper_UxROM() {}

protected:
    void cpu_write(uint16_t address, uint8_t data) override;
    void ppu_write(uint16_t address, uint8_t data) override;

private:
    uint8_t m_register = 0;

    void configure();
};

#pragma once

#include "mapper.hpp"

class Mapper_CNROM : public Mapper
{
public:
    Mapper_CNROM(NesRom& rom);
    virtual ~Mapper_CNROM() {}

protected:
    void cpu_write(uint16_t address, uint8_t data) override;
    void ppu_write(uint16_t address, uint8_t data) override;

private:
    uint8_t m_register = 0;

    void configure();
};
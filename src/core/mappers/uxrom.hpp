#pragma once

#include "mapper.hpp"

class UxROM : public Mapper
{
public:
    UxROM(NesFile& nes_file): Mapper(nes_file) {}
    virtual ~UxROM() {}

    std::string name() const override { return "UxROM"; }
    uint32_t read(uint16_t address) override;
    uint32_t write(uint16_t address, uint8_t data) override;
    bool irq() override { return false; }
    void irq_clear() override {}
    void scanline() override {}

private:
    uint8_t m_program_bank = 0;

    uint32_t map_address(uint16_t address) const;
};

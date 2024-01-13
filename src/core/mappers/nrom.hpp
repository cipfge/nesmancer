#pragma once

#include "mapper.hpp"

class NROM : public Mapper
{
public:
    NROM(NesFile& nes_file): Mapper(nes_file) {}
    virtual ~NROM() {}

    std::string name() const override { return "NROM"; }
    uint32_t read(uint16_t address) override;
    uint32_t write(uint16_t address, uint8_t data) override;
    bool irq() override { return false; }
    void irq_clear() override {}
    void scanline() override {}

private:
    uint32_t map_address(uint16_t address) const;
};
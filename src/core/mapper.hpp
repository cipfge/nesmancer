#pragma once

#include <cstdint>

enum MirroringMode
{
    MIRROR_HORIZONTAL,
    MIRROR_VERTICAL
};

class Mapper
{
public:


public:
    Mapper(uint16_t id, uint8_t prg_banks,
           uint8_t chr_banks, MirroringMode mirroring_mode);
    virtual ~Mapper();

    virtual uint32_t read(uint16_t address) = 0;
    virtual uint32_t write(uint16_t address, uint8_t data) = 0;
    virtual bool irq() = 0;
    virtual void irq_clear() = 0;
    virtual void scanline() = 0;

protected:
    uint16_t m_id = 0;
    uint8_t m_prg_banks = 0;
    uint8_t m_chr_banks = 0;
    MirroringMode m_mirroring_mode = MIRROR_HORIZONTAL;
};

// Mapper 000 - NROM
class Mapper0: public Mapper
{
public:
    Mapper0(uint8_t prg_banks, uint8_t chr_banks, MirroringMode mirroring_mode);
    virtual ~Mapper0();

    virtual uint32_t read(uint16_t address) override;
    virtual uint32_t write(uint16_t address, uint8_t data) override;
    virtual bool irq() override { return false; }
    virtual void irq_clear() override {}
    virtual void scanline() override {}

private:
    uint32_t map_address(uint16_t address);
};

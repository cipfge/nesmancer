#pragma once

#include "types.hpp"

class Mapper
{
public:
    enum MirrorMode
    {
        MIRROR_HORIZONTAL,
        MIRROR_VERTICAL
    };

public:
    Mapper(uint16_t id, uint8_t prg_banks,
           uint8_t chr_banks, MirrorMode mirror_mode);
    virtual ~Mapper();

    virtual uint32_t read(uint16_t address) = 0;
    virtual uint32_t write(uint16_t address, uint8_t data) = 0;
    virtual bool cpu_irq() = 0;

protected:
    uint16_t m_id = 0;
    uint8_t m_prg_banks = 0;
    uint8_t m_chr_banks = 0;
    MirrorMode m_mirror_mode = MIRROR_HORIZONTAL;
};

class Mapper0: public Mapper
{
public:
    Mapper0(uint8_t prg_banks, uint8_t chr_banks, MirrorMode mirror_mode);
    virtual ~Mapper0();

    virtual uint32_t read(uint16_t address) override;
    virtual uint32_t write(uint16_t address, uint8_t data) override;
    virtual bool cpu_irq() override;

private:
    uint32_t map_address(uint16_t address);
};

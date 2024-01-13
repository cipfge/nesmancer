#pragma once

#include "types.hpp"
#include "nes_file.hpp"
#include <cstdint>
#include <string>

class Mapper
{
public:
    Mapper(NesFile& nes_file):
        m_id(nes_file.mapper_id()),
        m_program_bank_count(nes_file.program_bank_count()),
        m_character_bank_count(nes_file.character_bank_count()),
        m_mirroring_mode(nes_file.mirroring_mode())
    {}

    virtual ~Mapper() {};

    uint16_t id() const { return m_id; }
    uint8_t prg_banks() const { return m_program_bank_count; }
    uint8_t chr_banks() const { return m_character_bank_count; }
    MirroringMode mirroring_mode() const { return m_mirroring_mode; }

    virtual std::string name() const = 0;
    virtual uint32_t read(uint16_t address) = 0;
    virtual uint32_t write(uint16_t address, uint8_t data) = 0;
    virtual bool irq() = 0;
    virtual void irq_clear() = 0;
    virtual void scanline() = 0;

protected:
    uint16_t m_id = 0;
    uint8_t m_program_bank_count = 0;
    uint8_t m_character_bank_count = 0;
    MirroringMode m_mirroring_mode = MirroringMode::Horizontal;
};

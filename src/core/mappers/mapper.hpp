#pragma once

#include <cstdint>
#include <string>

enum class MirroringMode
{
    Horizontal,
    Vertical,
    FourScreens
};

enum MapperId
{
    MAPPER_NROM = 0,
    MAPPER_MCC1,
    MAPPER_UXROM,
    MAPPER_CNROM,
    MAPPER_MMC3,
    MAPPER_6,
    MAPPER_MMC5,
    MAPPER_AXROM
};

class Mapper
{
public:
    Mapper(uint16_t id,
           uint8_t prg_bank_count,
           uint8_t chr_bank_count,
           MirroringMode mirroring_mode);

    virtual ~Mapper();

    uint16_t id() const { return m_id; }
    uint8_t prg_banks() const { return m_prg_bank_count; }
    uint8_t chr_banks() const { return m_chr_bank_count; }
    MirroringMode mirroring_mode() const { return m_mirroring_mode; }

    virtual uint32_t read(uint16_t address) = 0;
    virtual uint32_t write(uint16_t address, uint8_t data) = 0;
    virtual bool irq() = 0;
    virtual void irq_clear() = 0;
    virtual void scanline() = 0;
    virtual std::string name() const = 0;

    static constexpr uint32_t Size_1KB = 0x400;
    static constexpr uint32_t Size_4KB = 0x1000;
    static constexpr uint32_t Size_8KB = 0x2000;
    static constexpr uint32_t Size_16KB = 0x4000;
    static constexpr uint32_t Size_32KB = 0x8000;

protected:
    uint16_t m_id = 0;
    uint8_t m_prg_bank_count = 0;
    uint8_t m_chr_bank_count = 0;
    MirroringMode m_mirroring_mode = MirroringMode::Horizontal;
};


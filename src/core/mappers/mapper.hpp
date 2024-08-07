#pragma once

#include "nes_rom.hpp"
#include <cstdint>
#include <string>
#include <array>
#include <vector>

enum SupportedMappers
{
    MAPPER_NROM,
    MAPPER_MMC1,
    MAPPER_UXROM,
    MAPPER_CNROM,
    MAPPER_MMC3
};

class Mapper
{
public:
    Mapper(NesRom& rom);
    virtual ~Mapper() {};

    uint16_t id() const { return m_id; }
    MirroringMode mirroring_mode() { return m_mirroring_mode; }
    uint8_t cpu_read(uint16_t address);
    virtual void cpu_write(uint16_t address, uint8_t data) = 0;
    uint8_t ppu_read(uint16_t address);
    virtual void ppu_write(uint16_t address, uint8_t data) = 0;

    virtual bool irq() { return false; }
    virtual void irq_clear() {}
    virtual void scanline() {}

    static constexpr uint8_t MaxPrgBankCount = 4;
    static constexpr uint8_t MaxChrBankCount = 8;

protected:
    uint16_t m_id = 0;
    uint8_t m_prg_banks = 0;
    uint32_t m_prg_size = 0;
    uint32_t m_prg_ram_size = 0;
    uint8_t m_chr_banks = 0;
    uint32_t m_chr_size = 0;
    bool m_chr_ram = false;

    MirroringMode m_mirroring_mode = MirroringMode::Horizontal;

    std::array<uint32_t, MaxPrgBankCount> m_prg_mapping = {};
    std::array<uint32_t, MaxChrBankCount> m_chr_mapping = {};

    std::vector<uint8_t> m_prg;
    std::vector<uint8_t> m_prg_ram;
    std::vector<uint8_t> m_chr;

    template <int KB>
    void map_prg(int slot, int bank);

    template<int KB>
    void map_chr(int slot, int bank);
};

template <int KB>
void Mapper::map_prg(int slot, int bank)
{
    if (bank < 0)
        bank = (m_prg_size / (0x400 * KB)) + bank;

    for (int i = 0; i < (KB / 8); i++)
        m_prg_mapping[(KB / 8) * slot + i] = (KB * 0x400 * bank + 0x2000 * i) % m_prg_size;
}

template void Mapper::map_prg<32>(int, int);
template void Mapper::map_prg<16>(int, int);
template void Mapper::map_prg<8>(int, int);

template <int KB>
void Mapper::map_chr(int slot, int bank)
{
    for (int i = 0; i < KB; i++)
        m_chr_mapping[KB * slot + i] = (KB * 0x400 * bank + 0x400 * i) % m_chr_size;
}

template void Mapper::map_chr<8>(int, int);
template void Mapper::map_chr<4>(int, int);
template void Mapper::map_chr<2>(int, int);
template void Mapper::map_chr<1>(int, int);

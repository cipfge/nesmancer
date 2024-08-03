#pragma once

#include <cstdint>

enum class NesFileVersion
{
    iNes,
    Nes2,
    Unsupported
};

struct NesFileHeader
{
    static constexpr char ValidSignature[] = "NES\x1A";
    static constexpr uint16_t TrainerSize = 512;

    char signature[4] = {};
    uint8_t prg_bank_count = 0;
    uint8_t chr_bank_count = 0;
    uint8_t bytes[10] = {};
};

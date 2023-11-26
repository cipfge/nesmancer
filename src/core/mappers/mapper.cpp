#include "mapper.hpp"

Mapper::Mapper(uint16_t id,
               uint8_t prg_banks,
               uint8_t chr_banks,
               MirroringMode mirroring_mode)
    : m_id(id)
    , m_prg_banks(prg_banks)
    , m_chr_banks(chr_banks)
    , m_mirroring_mode(mirroring_mode)
{
}

Mapper::~Mapper()
{
}

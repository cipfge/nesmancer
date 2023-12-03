#include "mapper.hpp"

Mapper::Mapper(uint16_t id,
               uint8_t prg_bank_count,
               uint8_t chr_bank_count,
               MirroringMode mirroring_mode)
    : m_id(id)
    , m_prg_bank_count(prg_bank_count)
    , m_chr_bank_count(chr_bank_count)
    , m_mirroring_mode(mirroring_mode)
{
}

Mapper::~Mapper()
{
}

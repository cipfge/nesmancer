#include "apu.hpp"
#include "global.hpp"

APU::APU()
{
}

APU::~APU()
{
}

void APU::reset()
{
}

void APU::tick()
{
}

uint8_t APU::read(uint16_t address)
{
    EMU_UNUSED(address);
    return 0;
}

void APU::write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(address);
    EMU_UNUSED(data);
}

#include "apu.hpp"
#include "global.hpp"
#include "logger.hpp"

static int null_dmc_reader(int)
{
    return 0x55;
}

APU::APU()
{
    m_apu.dmc_reader = null_dmc_reader;
}

APU::~APU()
{
}

uint8_t APU::read()
{
    blip_time_t time = clock();
    return m_apu.read_status(time);
}

void APU::write(uint16_t address, uint8_t data)
{
    blip_time_t time = clock();
    m_apu.write_register(time, address, data);
}

void APU::dmc_reader(int (*f)(void* user_data, int addr), void* p)
{
    m_apu.dmc_reader = std::bind(f, p, std::placeholders::_1);
}

std::error_condition APU::sample_rate(long rate)
{
    m_apu.set_output(&m_buffer);
    m_buffer.clock_rate(1789773);

    return m_buffer.set_sample_rate(rate);
}

void APU::end_frame()
{
    m_time = 0;
    m_frame_length ^= 1;
    m_apu.end_frame(m_frame_length);
    m_buffer.end_frame(m_frame_length);
}

long APU::read_samples(blip_sample_t* buffer, long size)
{
    return m_buffer.read_samples(buffer, size);
}

blip_time_t APU::clock()
{
    return m_time += 4;
}

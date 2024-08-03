#include "apu.hpp"
#include "system_bus.hpp"
#include "global.hpp"
#include "logger.hpp"

APU::APU()
{
    m_apu.dmc_reader = [this](int address) -> int {
        if (!m_system_bus)
            return 0x55; // Flat dmc samples
        else
            return m_system_bus->read(address);
    };
}

void APU::set_system_bus(SystemBus* system_bus)
{
    m_system_bus = system_bus;
}

bool APU::init()
{
    m_apu.set_output(&m_buffer);
    m_buffer.clock_rate(NTSC_ClockRate);
    if (m_buffer.set_sample_rate(SoundSampleRate))
    {
        LOG_FATAL("APU error, cannot set sample rate %u", SoundSampleRate);
        return false;
    }

    return true;
}

void APU::reset()
{
    m_apu.reset();
    m_buffer.clear();
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
    return m_time += NTSC_ClockTick;
}

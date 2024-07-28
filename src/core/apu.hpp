#pragma once

#include "nes_apu/Nes_Apu.h"
#include "nes_apu/Blip_Buffer.h"

class APU
{
public:
    APU();

    void reset();
    uint8_t read();
    void write(uint16_t address, uint8_t data);
    void end_frame();

    std::error_condition set_sample_rate(long rate);

    long samples_available() const { return m_buffer.samples_avail(); }
    long read_samples(blip_sample_t* buffer, long size);

    static constexpr int ClockTick = 4;
    static constexpr long ClockRate = 1789773;
    static constexpr long FrameLength = 29780;

private:
    Nes_Apu m_apu;
    Blip_Buffer m_buffer;
    blip_time_t m_time = 0;
    blip_time_t m_frame_length = FrameLength;

    blip_time_t clock();
};

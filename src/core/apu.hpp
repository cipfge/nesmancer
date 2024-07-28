#pragma once

#include "nes_apu/Nes_Apu.h"
#include "nes_apu/Blip_Buffer.h"

class APU
{
public:
    APU();
    ~APU();

    uint8_t read();
    void write(uint16_t address, uint8_t data);

    void dmc_reader(int (*callback)(void* user_data, int), void* user_data = nullptr);
    std::error_condition sample_rate(long rate);
    void end_frame();

    long samples_available() const { return m_buffer.samples_avail(); }
    long read_samples(blip_sample_t* buffer, long size);

private:
    Nes_Apu m_apu;
    Blip_Buffer m_buffer;
    blip_time_t m_time = 0;
    blip_time_t m_frame_length = 29780;

    blip_time_t clock();
};

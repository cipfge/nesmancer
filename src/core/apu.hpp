#pragma once

#include "nes_apu/Nes_Apu.h"
#include "nes_apu/Blip_Buffer.h"

class SystemBus;

class APU
{
public:
    APU();

    void set_system_bus(SystemBus* system_bus);
    bool init();

    void reset();
    uint8_t read();
    void write(uint16_t address, uint8_t data);
    void end_frame();

    long samples_available() const { return m_buffer.samples_avail(); }
    long read_samples(blip_sample_t* buffer, long size);

    static constexpr int NTSC_ClockTick = 4;
    static constexpr int PAL_ClockTick = 5;

    static constexpr long NTSC_ClockRate = 1789773; // 1.789773 MHz
    static constexpr long NTSC_FrameLength = 29780;

    static constexpr long PAL_ClockRate = 1662607; // 1.662607 MHz
    static constexpr long PAL_FrameLength = 33247;

    static constexpr long SoundSampleRate = 44100;
    static constexpr long SoundBufferSize = 4096;

private:
    SystemBus* m_system_bus = nullptr;
    Nes_Apu m_apu;
    Blip_Buffer m_buffer;
    blip_time_t m_time = 0;
    blip_time_t m_frame_length = PAL_FrameLength;

    blip_time_t clock();
};

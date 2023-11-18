#pragma once

#include <SDL.h>
#include <cstdint>

class APU
{
public:
    APU();
    ~APU();

    bool init_audio_device();
    void reset();
    void tick();

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    SDL_AudioSpec m_spec_desired;
    SDL_AudioSpec m_spec_obtained;
    SDL_AudioDeviceID m_audio_device;
};

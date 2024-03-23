#include "apu.hpp"
#include "global.hpp"
#include "logger.hpp"

APU::APU()
{
}

APU::~APU()
{
    SDL_CloseAudioDevice(m_audio_device);
}

bool APU::init_audio_device()
{
    m_spec_desired.freq = 44100;
    m_spec_desired.format = AUDIO_F32;
    m_spec_desired.channels = 1;
    m_spec_desired.samples = 512;
    m_spec_desired.userdata = this;

    m_spec_desired.callback = [](void *userdata, uint8_t *buffer, int size)
    {
        EMU_UNUSED(userdata);
        EMU_UNUSED(buffer);
        EMU_UNUSED(size);
    };

    m_audio_device = SDL_OpenAudioDevice(nullptr, 0,  &m_spec_desired, &m_spec_obtained, 0);
    if (!m_audio_device)
    {
        LOG_ERROR("SDL_OpenAudioDevice error: %s", SDL_GetError());
        return false;
    }
    else if (m_spec_desired.format != m_spec_obtained.format)
    {
        LOG_ERROR("Failed to obtain desired audio format");
        return false;
    }

    SDL_PauseAudioDevice(m_audio_device, 0);

    return true;
}

void APU::reset()
{
}

void APU::tick()
{
}

uint8_t APU::read(uint16_t address)
{
    uint8_t data = 0;

    switch (address)
    {
    // Status
    case 0x4015:
        break;

    default:
        break;
    }

    return data;
}

void APU::write(uint16_t address, uint8_t data)
{
    EMU_UNUSED(data);

    switch (address)
    {
    // Pulse 1 channel: $4000 - $4003
    case 0x4000:
        break;
    case 0x4001:
        break;
    case 0x4002:
        break;
    case 0x4003:
        break;

    // Pulse 2 channel: $4004 - $4007
    case 0x4004:
        break;
    case 0x4005:
        break;
    case 0x4006:
        break;
    case 0x4007:
        break;

    // Triangle channel: $4008, $400A & $400B
    case 0x4008:
        break;
    case 0x400A:
        break;
    case 0x400B:
        break;

    // Noise channel: $400C, $400E & $400F
    case 0x400C:
        break;
    case 0x400E:
        break;
    case 0x400F:
        break;

    // DMC channel: $4010 - $4013
    case 0x4010:
        break;
    case 0x4011:
        break;
    case 0x4012:
        break;
    case 0x4013:
        break;

    default:
        break;
    }
}

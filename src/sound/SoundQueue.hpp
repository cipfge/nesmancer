#pragma once

#include <SDL.h>
#include <cstdint>

class SoundQueue {
public:
    SoundQueue() = default;
    ~SoundQueue();
    
    bool init(long sample_rate, int channel_count = 1);
    int sample_count() const;

    typedef short sample_t;
    void write(const sample_t*, int count);

private:
    static constexpr long BufferSize = 2048;
    static constexpr int BufferCount = 3;

    SDL_AudioSpec m_audio_spec = {};
    sample_t* volatile m_buffers = nullptr;
    SDL_sem* volatile m_semaphore = nullptr;
    int volatile m_read_buffer = 0;
    int m_write_buffer = 0;
    int m_write_position = 0;

    sample_t* get_buffer(int index);
    void fill_buffer(uint8_t* stream, int size);
};

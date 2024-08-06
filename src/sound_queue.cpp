#include "sound_queue.hpp"
#include "logger.hpp"
#include <string>
#include <assert.h>

SoundQueue::~SoundQueue()
{
    SDL_PauseAudio(true);
    SDL_CloseAudio();
    SDL_DestroySemaphore(m_semaphore);

    delete [] m_buffers;
}

bool SoundQueue::init(long sample_rate, int channel_count)
{
    assert(!m_buffers);

    m_buffers = new sample_t[BufferSize * BufferCount];
    if (!m_buffers)
     {
        LOG_FATAL("Cannot allocate memory for sound queue buffers");
        return false;
    }

    m_semaphore = SDL_CreateSemaphore(BufferCount - 1);
    if (!m_semaphore)
    {
        LOG_FATAL("SDL_CreateSemaphore error: %s", SDL_GetError());
        return false;
    }

    m_audio_spec.freq = sample_rate;
    m_audio_spec.format = AUDIO_S16SYS;
    m_audio_spec.channels = channel_count;
    m_audio_spec.silence = 0;
    m_audio_spec.samples = BufferSize;
    m_audio_spec.size = 0;
    m_audio_spec.userdata = this;
    m_audio_spec.callback = [](void* user_data, unsigned char* stream, int size) {
        SoundQueue* sq = reinterpret_cast<SoundQueue*>(user_data);
        sq->fill_buffer(stream, size);
    };

    if (SDL_OpenAudio(&m_audio_spec, nullptr) < 0)
    {
        LOG_FATAL("SDL_OpenAudio error: %s", SDL_GetError());
        return false;
    }

    SDL_PauseAudio(false);

    return true;
}

int SoundQueue::sample_count() const
{
    int buffer_free = SDL_SemValue(m_semaphore) * BufferSize + (BufferSize - m_write_position);
    return BufferSize * BufferCount - buffer_free;
}

inline SoundQueue::sample_t* SoundQueue::get_buffer(int index)
{
    assert(static_cast<unsigned>(index) < BufferCount);
    return m_buffers + static_cast<long>(index) * BufferSize;
}

void SoundQueue::write(const sample_t* stream, int size)
{
    while (size)
    {
        int n = BufferSize - m_write_position;
        if (n > size)
            n = size;
        
        memcpy(get_buffer(m_write_buffer) + m_write_position, stream, n * sizeof(sample_t));
        stream += n;
        m_write_position += n;
        size -= n;
        
        if (m_write_position >= BufferSize)
        {
            m_write_position = 0;
            m_write_buffer = (m_write_buffer + 1) % BufferCount;
            SDL_SemWait(m_semaphore);
        }
    }
}

void SoundQueue::fill_buffer(uint8_t* stream, int size)
{
    if (SDL_SemValue(m_semaphore) < BufferCount - 1)
    {
        memcpy(stream, get_buffer(m_read_buffer), size);
        m_read_buffer = (m_read_buffer + 1) % BufferCount;
        SDL_SemPost(m_semaphore);
    }
    else
    {
        memset(stream, 0, size);
    }
}

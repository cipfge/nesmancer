#include "logger.hpp"
#include "platform.hpp"
#include "version.hpp"
#include <cstdarg>
#include <string>
#include <sstream>
#include <iostream>

#ifdef EMU_PLATFORM_WINDOWS
#include <Windows.h>
#include <debugapi.h>
#endif // Windows

inline void log_write_error(const std::string& message)
{
#ifndef EMU_DEBUG_ENABLED
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, EMU_VERSION_NAME, message.c_str(), nullptr);
#elif EMU_PLATFORM_WINDOWS
    OutputDebugStringA(message.c_str());
#else
    std::cerr << message;
#endif // Debug enabled and platform
}

inline void log_write(const std::string& message)
{
#ifdef EMU_PLATFORM_WINDOWS
    OutputDebugStringA(message.c_str());
#else
    std::cout << message;
#endif // Windows
}

void log_f(LogLevel level, const char* fmt, ...)
{
    constexpr std::size_t LOG_BUFFER_SIZE = 1024;
    static const char* level_names[LOG_LEVEL_MAX] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "FATAL"
    };

    va_list args;
    va_start(args, fmt);

    char buffer[LOG_BUFFER_SIZE] = {0};
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::stringstream ss;
    ss << "[" << level_names[level] << "]: " <<  buffer << "\n";

    if (level >= LOG_LEVEL_ERROR)
        log_write_error(ss.str());
    else
        log_write(ss.str());
}

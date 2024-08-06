#pragma once

#ifdef EMU_DEBUG_ENABLED
#define LOG_DEBUG(message, ...) log_f(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
#define LOG_DEBUG(message, ...)
#endif // Debug enabled

#define LOG_INFO(message, ...)    log_f(LOG_LEVEL_INFO,    message, ##__VA_ARGS__)
#define LOG_WARNING(message, ...) log_f(LOG_LEVEL_WARNING, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...)   log_f(LOG_LEVEL_ERROR,   message, ##__VA_ARGS__)
#define LOG_FATAL(message, ...)   log_f(LOG_LEVEL_FATAL,   message, ##__VA_ARGS__)

enum LogLevel
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_MAX
};

void log_f(LogLevel level, const char* fmt, ...);

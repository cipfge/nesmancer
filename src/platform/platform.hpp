#pragma once

#if defined(_WIN64) || defined(_WIN32)
#define EMU_PLATFORM_WINDOWS
#elif defined(__linux__)
#define EMU_PLATFORM_LINUX
#elif defined(__APPLE__)
#define EMU_PLATFORM_MACOS
#else
#error "Unsupported platform detected"
#endif // Platform detection macros

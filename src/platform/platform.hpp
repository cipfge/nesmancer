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

#include <string>
#include <filesystem>

namespace platform
{

inline uint32_t file_size(const std::string& file_path)
{
    std::filesystem::path path{ file_path };
    return std::filesystem::file_size(path);
}

inline std::string file_name(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

inline std::string file_remove_extension(const std::string& file_name)
{
    std::string::size_type const p(file_name.find_last_of('.'));
    return ((p > 0) && (p != std::string::npos)) ? file_name.substr(0, p) : file_name;
}

} // namespace platform

#pragma once

#include <string>

inline std::string path_get_filename(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

inline std::string filename_remove_extension(const std::string filename)
{
    std::string::size_type const p(filename.find_last_of('.'));
    return ((p > 0) && (p != std::string::npos)) ? filename.substr(0, p) : filename;
}

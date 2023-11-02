#pragma once

#include "types.hpp"
#include "cartridge.hpp"

class Device
{
public:
    Device() = default;
    ~Device();

    bool load_rom_file(const std::string path);

private:
    Cartridge m_cartridge;
};

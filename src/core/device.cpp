#include "device.hpp"

Device::~Device()
{
}

bool Device::load_rom_file(const std::string path)
{
    return m_cartridge.load_from_file(path);
}

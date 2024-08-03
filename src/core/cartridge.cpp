#include "cartridge.hpp"
#include "mapper_cnrom.hpp"
#include "mapper_nrom.hpp"
#include "mapper_mmc1.hpp"
#include "mapper_mmc3.hpp"
#include "mapper_uxrom.hpp"
#include "logger.hpp"
#include <stdexcept>

void Cartridge::reset()
{
    m_loaded = false;
}

bool Cartridge::load_from_file(const std::string& file_path)
{
    try
    {
        NesRom rom(file_path);
        switch (rom.mapper_id())
        {
        case 0:
            m_mapper = std::make_unique<Mapper_NROM>(rom);
            break;

        case 1:
            m_mapper = std::make_unique<Mapper_MMC1>(rom);
            break;

        case 2:
            m_mapper = std::make_unique<Mapper_UxROM>(rom);
            break;

        case 3:
            m_mapper = std::make_unique<Mapper_CNROM>(rom);
            break;

        case 4:
            m_mapper = std::make_unique<Mapper_MMC3>(rom);
            break;

        default:
            LOG_ERROR("Unsupported mapper id %u", rom.mapper_id());
            return false;
        }

        m_loaded = true;
    }
    catch (std::runtime_error e)
    {
        LOG_ERROR("Error loading file %s: %s", file_path, e.what());
        return false;
    }

    return true;
}

MirroringMode Cartridge::mirroring_mode()
{
    if (!m_mapper)
        return MirroringMode::Horizontal;
    return m_mapper->mirroring_mode();
}

uint8_t Cartridge::cpu_read(uint16_t address)
{
    return m_mapper->cpu_read(address);
}

void Cartridge::cpu_write(uint16_t address, uint8_t data)
{
    m_mapper->cpu_write(address, data);
}

uint8_t Cartridge::ppu_read(uint16_t address)
{
    return m_mapper->ppu_read(address);
}

void Cartridge::ppu_write(uint16_t address, uint8_t data)
{
    m_mapper->ppu_write(address, data);
}

bool Cartridge::irq()
{
    if (!m_mapper)
        return false;
    return m_mapper->irq();
}

void Cartridge::irq_clear()
{
    if (m_mapper)
        m_mapper->irq_clear();
}

void Cartridge::scanline()
{
    if (m_mapper)
        m_mapper->scanline();
}

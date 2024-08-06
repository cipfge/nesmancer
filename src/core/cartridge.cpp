#include "cartridge.hpp"
#include "mapper_cnrom.hpp"
#include "mapper_nrom.hpp"
#include "mapper_mmc1.hpp"
#include "mapper_mmc3.hpp"
#include "mapper_uxrom.hpp"
#include "logger.hpp"
#include <stdexcept>
#include <cassert>

void Cartridge::reset()
{
    m_mapper.reset();
}

bool Cartridge::load_from_file(const std::string& file_path)
{
    try
    {
        NesRom rom(file_path);

        switch (rom.mapper_id())
        {
        case MAPPER_NROM:
            m_mapper = std::make_unique<Mapper_NROM>(rom);
            break;

        case MAPPER_MMC1:
            m_mapper = std::make_unique<Mapper_MMC1>(rom);
            break;

        case MAPPER_UXROM:
            m_mapper = std::make_unique<Mapper_UxROM>(rom);
            break;

        case MAPPER_CNROM:
            m_mapper = std::make_unique<Mapper_CNROM>(rom);
            break;

        case MAPPER_MMC3:
            m_mapper = std::make_unique<Mapper_MMC3>(rom);
            break;

        default:
            LOG_ERROR("Unsupported mapper id %u", rom.mapper_id());
            return false;
        }

        return true;
    }
    catch (std::runtime_error e)
    {
        LOG_ERROR("Error loading file %s: %s", file_path, e.what());
        return false;
    }
}

MirroringMode Cartridge::mirroring_mode()
{
    assert(m_mapper);
    return m_mapper->mirroring_mode();
}

uint8_t Cartridge::cpu_read(uint16_t address)
{
    assert(m_mapper);
    return m_mapper->cpu_read(address);
}

void Cartridge::cpu_write(uint16_t address, uint8_t data)
{
    assert(m_mapper);
    m_mapper->cpu_write(address, data);
}

uint8_t Cartridge::ppu_read(uint16_t address)
{
    assert(m_mapper);
    return m_mapper->ppu_read(address);
}

void Cartridge::ppu_write(uint16_t address, uint8_t data)
{
    assert(m_mapper);
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

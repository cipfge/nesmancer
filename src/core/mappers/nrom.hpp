#include "mapper.hpp"

class NROM : public Mapper
{
public:
    NROM(uint8_t prg_banks,
         uint8_t chr_banks,
         MirroringMode mirroring_mode);

    virtual ~NROM();

    virtual uint32_t read(uint16_t address) override;
    virtual uint32_t write(uint16_t address, uint8_t data) override;
    virtual bool irq() override { return false; }
    virtual void irq_clear() override {}
    virtual void scanline() override {}

private:
    uint32_t map_address(uint16_t address);
};
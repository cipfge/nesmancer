#include "cpu.hpp"
#include "memory.hpp"

CPU::CPU(Memory* memory)
    : m_memory(memory)
{
}

CPU::~CPU()
{
}

void CPU::reset()
{
    m_registers.A = 0;
    m_registers.X = 0;
    m_registers.Y = 0;
    m_registers.P = STATUS_I | STATUS_U;
    m_registers.SP = 0xFD;
    m_registers.PC = m_memory->read(0xFFFC) | (m_memory->read(0xFFFD) << 8);

    m_opcode = 0;
    m_address = 0;
    m_cycles = 7;
}

void CPU::set_flag(StatusFlags flag, bool value)
{
    m_registers.P = value ? m_registers.P | flag : m_registers.P & (~flag);
}

bool CPU::check_flag(StatusFlags flag)
{
    return (m_registers.P & flag) != 0;
}

void CPU::stack_push(uint8_t data)
{
    m_memory->write(0x100 | m_registers.SP, data);
    m_registers.SP--;
}

uint8_t CPU::stack_pop()
{
    m_registers.SP++;
    return m_memory->read(0x100 | m_registers.SP);
}

uint8_t CPU::am_implied()
{
    return 0;
}

uint8_t CPU::am_immediate()
{
    m_address = m_registers.PC++;
    return 0;
}

uint8_t CPU::am_absolute()
{
    Word address;
    address.byte_low = m_memory->read(m_registers.PC++);
    address.byte_high = m_memory->read(m_registers.PC++);
    m_address = address.value;

    return 0;
}

uint8_t CPU::am_absolute_x()
{
    Word address;
    address.byte_low = m_memory->read(m_registers.PC++);
    address.byte_high = m_memory->read(m_registers.PC++);
    m_address = address.value + m_registers.X;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
        return 1;

    return 0;
}

uint8_t CPU::am_absolute_y()
{
    Word address;
    address.byte_low = m_memory->read(m_registers.PC++);
    address.byte_high = m_memory->read(m_registers.PC++);
    m_address = address.value + m_registers.Y;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
        return 1;

    return 0;
}

uint8_t CPU::am_relative()
{
    int8_t offset = m_memory->read(m_registers.PC++);
    m_address = m_registers.PC + offset;

    if ((m_registers.PC & 0xFF00) != (m_address & 0xFF00))
        return 1;

    return 0;
}

uint8_t CPU::am_zeropage()
{
    m_address = m_memory->read(m_registers.PC++);
    return 0;
}

uint8_t CPU::am_zeropage_x()
{
    m_address = m_memory->read(m_registers.PC++) + m_registers.X;
    m_address &= 0xFF;
    return 0;
}

uint8_t CPU::am_zeropage_y()
{
    m_address = m_memory->read(m_registers.PC++) + m_registers.Y;
    m_address &= 0xFF;
    return 0;
}

uint8_t CPU::am_indirect()
{
    Word pointer;
    pointer.byte_low = m_memory->read(m_registers.PC++);
    pointer.byte_high = m_memory->read(m_registers.PC++);

    // 6502 page boundary hardware bug
    uint16_t byte_high = pointer.byte_low == 0xFF ? pointer.value & 0xFF00 : pointer.value + 1;

    Word address;
    address.byte_low = m_memory->read(pointer.value);
    address.byte_high = m_memory->read(byte_high);
    m_address = address.value;

    return 0;
}

uint8_t CPU::am_indexed_indirect()
{
    uint8_t zpg = m_registers.X + m_memory->read(m_registers.PC++);

    Word address;
    address.byte_low = m_memory->read(zpg++);
    address.byte_high = m_memory->read(zpg);
    m_address = address.value;

    return 0;
}

uint8_t CPU::am_indirect_indexed()
{
    uint8_t zpg = m_memory->read(m_registers.PC++);

    Word address;
    address.byte_low = m_memory->read(zpg++);
    address.byte_high = m_memory->read(zpg);
    m_address = address.value + m_registers.Y;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
        return 1;

    return 0;
}

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

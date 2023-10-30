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

uint8_t CPU::op_bcs()
{
    if (!check_flag(STATUS_C))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bcc()
{
    if (check_flag(STATUS_C))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_beq()
{
    if (!check_flag(STATUS_Z))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bne()
{
    if (check_flag(STATUS_Z))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bmi()
{
    if (!check_flag(STATUS_N))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bpl()
{
    if (check_flag(STATUS_N))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bvs()
{
    if (!check_flag(STATUS_V))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_bvc()
{
    if (check_flag(STATUS_V))
        return 0;

    m_cycles++;
    m_registers.PC = m_address;

    return 1;
}

uint8_t CPU::op_pha()
{
    stack_push(m_registers.A);
    return 0;
}

uint8_t CPU::op_php()
{
    stack_push(m_registers.P | STATUS_U | STATUS_B);
    return 0;
}

uint8_t CPU::op_pla()
{
    m_registers.A = stack_pop();
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_plp()
{
    m_registers.P = (stack_pop() | STATUS_U) & ~STATUS_B;
    return 0;
}

uint8_t CPU::op_inc()
{
    uint8_t result = m_memory->read(m_address) + 1;
    set_flag(STATUS_N, result >> 7);
    set_flag(STATUS_Z, result == 0);
    m_memory->write(m_address, result);

    return 0;
}

uint8_t CPU::op_inx()
{
    m_registers.X++;
    set_flag(STATUS_N, m_registers.X >> 7);
    set_flag(STATUS_Z, m_registers.X == 0);

    return 0;
}

uint8_t CPU::op_iny()
{
    m_registers.Y++;
    set_flag(STATUS_N, m_registers.Y >> 7);
    set_flag(STATUS_Z, m_registers.Y == 0);

    return 0;
}

uint8_t CPU::op_dec()
{
    uint8_t result = m_memory->read(m_address) - 1;
    set_flag(STATUS_N, result >> 7);
    set_flag(STATUS_Z, result == 0);
    m_memory->write(m_address, result);

    return 0;
}

uint8_t CPU::op_dex()
{
    m_registers.X--;
    set_flag(STATUS_N, m_registers.X >> 7);
    set_flag(STATUS_Z, m_registers.X == 0);

    return 0;
}

uint8_t CPU::op_dey()
{
    m_registers.Y--;
    set_flag(STATUS_N, m_registers.Y >> 7);
    set_flag(STATUS_Z, m_registers.Y == 0);

    return 0;
}

uint8_t CPU::op_adc()
{
    uint8_t operand = m_memory->read(m_address);
    int sign = (m_registers.A >> 7) == (operand >> 7);
    uint16_t value = m_registers.A + operand + (check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    uint8_t overflow = sign && (m_registers.A >> 7) != (operand >> 7);

    set_flag(STATUS_C, (value & 0x100) >> 8);
    set_flag(STATUS_Z, m_registers.A == 0);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_V, overflow);

    return 1;
}

uint8_t CPU::op_sbc()
{
    uint8_t operand = m_memory->read(m_address) ^ 0xFF;
    int sign = (m_registers.A & 0x80) == (operand & 0x80);
    uint16_t value = m_registers.A + operand + (check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    uint8_t overflow = sign && (m_registers.A & 0x80) != (operand & 0x80);

    set_flag(STATUS_C, (value & 0x100) >> 8);
    set_flag(STATUS_Z, m_registers.A == 0);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_V, overflow);

    return 1;
}

uint8_t CPU::op_clc()
{
    set_flag(STATUS_C, false);
    return 0;
}

uint8_t CPU::op_cld()
{
    set_flag(STATUS_D, false);
    return 0;
}

uint8_t CPU::op_cli()
{
    set_flag(STATUS_I, false);
    return 0;
}

uint8_t CPU::op_clv()
{
    set_flag(STATUS_V, false);
    return 0;
}

uint8_t CPU::op_sec()
{
    set_flag(STATUS_C, true);
    return 0;
}

uint8_t CPU::op_sed()
{
    set_flag(STATUS_D, true);
    return 0;
}

uint8_t CPU::op_sei()
{
    set_flag(STATUS_I, true);
    return 0;
}

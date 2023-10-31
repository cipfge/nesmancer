#include "cpu.hpp"
#include "memory.hpp"
#include <iostream>

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
    uint8_t value = m_memory->read(m_address) + 1;
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);
    m_memory->write(m_address, value);

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
    uint8_t value = m_memory->read(m_address) - 1;
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);
    m_memory->write(m_address, value);

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

uint8_t CPU::op_lda()
{
    m_registers.A = m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 1;
}

uint8_t CPU::op_ldx()
{
    m_registers.X = m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.X >> 7);
    set_flag(STATUS_Z, m_registers.X == 0);

    return 1;
}

uint8_t CPU::op_ldy()
{
    m_registers.Y = m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.Y >> 7);
    set_flag(STATUS_Z, m_registers.Y == 0);

    return 1;
}

uint8_t CPU::op_sta()
{
    m_memory->write(m_address, m_registers.A);
    return 0;
}

uint8_t CPU::op_stx()
{
    m_memory->write(m_address, m_registers.X);
    return 0;
}

uint8_t CPU::op_sty()
{
    m_memory->write(m_address, m_registers.Y);
    return 0;
}

uint8_t CPU::op_tax()
{
    m_registers.X = m_registers.A;
    set_flag(STATUS_N, m_registers.X >> 7);
    set_flag(STATUS_Z, m_registers.X == 0);

    return 0;
}

uint8_t CPU::op_tay()
{
    m_registers.Y = m_registers.A;
    set_flag(STATUS_N, m_registers.Y >> 7);
    set_flag(STATUS_Z, m_registers.Y == 0);

    return 0;
}

uint8_t CPU::op_tsx()
{
    m_registers.X = m_registers.SP;
    set_flag(STATUS_N, m_registers.X >> 7);
    set_flag(STATUS_Z, m_registers.X == 0);

    return 0;
}

uint8_t CPU::op_txa()
{
    m_registers.A = m_registers.X;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_txs()
{
    m_registers.SP = m_registers.X;
    return 0;
}

uint8_t CPU::op_tya()
{
    m_registers.A = m_registers.Y;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_jmp()
{
    m_registers.PC = m_address;
    return 0;
}

uint8_t CPU::op_rts()
{
    Word address;
    address.byte_low = stack_pop();
    address.byte_high = stack_pop();
    m_registers.PC = address.value + 1;

    return 0;
}

uint8_t CPU::op_jsr()
{
    Word address;
    address.value = m_registers.PC - 1;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    m_registers.PC = m_address;

    return 0;
}

uint8_t CPU::op_brk()
{
    Word address;
    address.value = m_registers.PC + 1;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push(m_registers.P | STATUS_U | STATUS_B);
    m_registers.PC = m_memory->read(0xFFFE) | (m_memory->read(0xFFFF) << 8);
    set_flag(STATUS_I, true);

    return 0;
}

uint8_t CPU::op_rti()
{
    m_registers.P = (stack_pop() | STATUS_U) & ~STATUS_B;

    Word address;
    address.byte_low = stack_pop();
    address.byte_high = stack_pop();
    m_registers.PC = address.value;

    return 0;
}

uint8_t CPU::op_cmp()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = m_registers.A - operand;
    set_flag(STATUS_C, m_registers.A >= operand);
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);

    return 1;
}

uint8_t CPU::op_cpx()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = m_registers.X - operand;
    set_flag(STATUS_C, m_registers.X >= operand);
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);

    return 1;
}

uint8_t CPU::op_cpy()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = m_registers.Y - operand;
    set_flag(STATUS_C, m_registers.Y >= operand);
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);

    return 1;
}

uint8_t CPU::op_and()
{
    m_registers.A &= m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 1;
}

uint8_t CPU::op_eor()
{
    m_registers.A ^= m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 1;
}

uint8_t CPU::op_ora()
{
    m_registers.A |= m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 1;
}

uint8_t CPU::op_nop()
{
    return 1;
}

uint8_t CPU::op_hlt()
{
    std::cerr << "Illegal instruction " << (unsigned)m_opcode << " executed, Halt!";
    exit(-1);
    return 0;
}

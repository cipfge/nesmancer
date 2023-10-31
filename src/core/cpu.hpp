#pragma once

#include <cstdint>

class Memory;

class CPU
{
public:
    enum StatusFlags: uint8_t
    {
        STATUS_C = (1 << 0),
        STATUS_Z = (1 << 1),
        STATUS_I = (1 << 2),
        STATUS_D = (1 << 3),
        STATUS_B = (1 << 4),
        STATUS_U = (1 << 5),
        STATUS_V = (1 << 6),
        STATUS_N = (1 << 7)
    };

    struct Registers
    {
        uint8_t A = 0;
        uint8_t X = 0;
        uint8_t Y = 0;
        uint8_t P = 0;
        uint8_t SP = 0;
        uint16_t PC = 0;
    };

    enum AddressingMode
    {
        AM_IMPLIED,
        AM_IMMEDIATE,
        AM_ABSOLUTE,
        AM_ABSOLUTE_INDEXED_X,
        AM_ABSOLUTE_INDEXED_Y,
        AM_RELATIVE,
        AM_ZEROPAGE,
        AM_ZEROPAGE_INDEXED_X,
        AM_ZEROPAGE_INDEXED_Y,
        AM_INDIRECT,
        AM_INDEXED_INDIRECT,
        AM_INDIRECT_INDEXED
    };

public:
    CPU(Memory* memory);
    ~CPU();

    void reset();

private:
    Registers m_registers;
    Memory* m_memory = nullptr;
    uint8_t m_opcode = 0;
    AddressingMode m_addressing_mode = AM_IMPLIED;
    uint16_t m_address = 0;
    uint8_t m_cycles = 0;

    void set_flag(StatusFlags flag, bool value);
    bool check_flag(StatusFlags flag);

    void stack_push(uint8_t data);
    uint8_t stack_pop();

    // Addressing modes
    uint8_t am_implied();
    uint8_t am_immediate();
    uint8_t am_absolute();
    uint8_t am_absolute_x();
    uint8_t am_absolute_y();
    uint8_t am_relative();
    uint8_t am_zeropage();
    uint8_t am_zeropage_x();
    uint8_t am_zeropage_y();
    uint8_t am_indirect();
    uint8_t am_indexed_indirect();
    uint8_t am_indirect_indexed();

    // Conditional branch opcodes
    uint8_t op_bcs();
    uint8_t op_bcc();
    uint8_t op_beq();
    uint8_t op_bne();
    uint8_t op_bmi();
    uint8_t op_bpl();
    uint8_t op_bvs();
    uint8_t op_bvc();

    // Stack opcodes
    uint8_t op_pha();
    uint8_t op_php();
    uint8_t op_pla();
    uint8_t op_plp();

    // Increment & decrement opcodes
    uint8_t op_inc();
    uint8_t op_inx();
    uint8_t op_iny();
    uint8_t op_dec();
    uint8_t op_dex();
    uint8_t op_dey();

    // Arithmetic opcodes
    uint8_t op_adc();
    uint8_t op_sbc();

    // Flag opcodes
    uint8_t op_clc();
    uint8_t op_cld();
    uint8_t op_cli();
    uint8_t op_clv();
    uint8_t op_sec();
    uint8_t op_sed();
    uint8_t op_sei();

    // Transfer opcodes
    uint8_t op_lda();
    uint8_t op_ldx();
    uint8_t op_ldy();
    uint8_t op_sta();
    uint8_t op_stx();
    uint8_t op_sty();
    uint8_t op_tax();
    uint8_t op_tay();
    uint8_t op_tsx();
    uint8_t op_txa();
    uint8_t op_txs();
    uint8_t op_tya();

    // Jumps & subroutines
    uint8_t op_jmp();
    uint8_t op_rts();
    uint8_t op_jsr();
    uint8_t op_brk();
    uint8_t op_rti();

    // Comparisons
    uint8_t op_cmp();
    uint8_t op_cpx();
    uint8_t op_cpy();

    // Logical opcodes
    uint8_t op_and();
    uint8_t op_eor();
    uint8_t op_ora();

    // Shift & rotate opcodes
    uint8_t op_asl();
    uint8_t op_lsr();
    uint8_t op_rol();
    uint8_t op_ror();

    // Other opcodes
    uint8_t op_bit();
    uint8_t op_nop();
    uint8_t op_hlt();
};

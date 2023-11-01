#pragma once

#include "types.hpp"
#include <string>

class Memory;

class CPU
{
public:
    enum StatusFlag: uint8_t
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

    struct Opcode
    {
        std::string mnemonic = "";
        AddressingMode addressing_mode = AddressingMode::AM_IMPLIED;
        bool (CPU::*execute)(void) = nullptr;
        bool (CPU::*read_address)(void) = nullptr;
        uint8_t cycles = 0;
    };

public:
    CPU(Memory* memory);
    ~CPU();

    void reset();
    void irq();
    void nmi();
    void tick();

private:
    Registers m_registers;
    Memory* m_memory = nullptr;
    uint8_t m_opcode = 0;
    AddressingMode m_addressing_mode = AM_IMPLIED;
    uint16_t m_address = 0;
    uint8_t m_cycles = 0;
    static Opcode m_opcode_table[256];

    void set_flag(StatusFlag flag, bool value);
    bool check_flag(StatusFlag flag);

    void stack_push(uint8_t data);
    uint8_t stack_pop();

    // Addressing modes
    bool read_implied();
    bool read_immediate();
    bool read_absolute();
    bool read_absolute_x();
    bool read_absolute_y();
    bool read_relative();
    bool read_zeropage();
    bool read_zeropage_x();
    bool read_zeropage_y();
    bool read_indirect();
    bool read_indexed_indirect();
    bool read_indirect_indexed();

    // Opcodes
    bool op_bcs();
    bool op_bcc();
    bool op_beq();
    bool op_bne();
    bool op_bmi();
    bool op_bpl();
    bool op_bvs();
    bool op_bvc();
    bool op_pha();
    bool op_php();
    bool op_pla();
    bool op_plp();
    bool op_inc();
    bool op_inx();
    bool op_iny();
    bool op_dec();
    bool op_dex();
    bool op_dey();
    bool op_adc();
    bool op_sbc();
    bool op_clc();
    bool op_cld();
    bool op_cli();
    bool op_clv();
    bool op_sec();
    bool op_sed();
    bool op_sei();
    bool op_lda();
    bool op_ldx();
    bool op_ldy();
    bool op_sta();
    bool op_stx();
    bool op_sty();
    bool op_tax();
    bool op_tay();
    bool op_tsx();
    bool op_txa();
    bool op_txs();
    bool op_tya();
    bool op_jmp();
    bool op_rts();
    bool op_jsr();
    bool op_brk();
    bool op_rti();
    bool op_cmp();
    bool op_cpx();
    bool op_cpy();
    bool op_and();
    bool op_eor();
    bool op_ora();
    bool op_asl();
    bool op_lsr();
    bool op_rol();
    bool op_ror();
    bool op_bit();
    bool op_lax();
    bool op_sax();
    bool op_dcp();
    bool op_isc();
    bool op_slo();
    bool op_rla();
    bool op_sre();
    bool op_rra();
    bool op_anc();
    bool op_alr();
    bool op_arr();
    bool op_xaa();
    bool op_las();
    bool op_ahx();
    bool op_tas();
    bool op_shy();
    bool op_shx();
    bool op_nop();
    bool op_hlt();
};

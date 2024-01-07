#pragma once

#include <cstdint>
#include <string>

class Memory;

class CPU
{
public:
    enum StatusFlag : uint8_t
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

    static constexpr uint8_t INT_Cycles = 7;

    static constexpr uint16_t NMI_Vector = 0xFFFA;
    static constexpr uint16_t RST_Vector = 0xFFFC;
    static constexpr uint16_t IRQ_Vector = 0xFFFE;

public:
    CPU(Memory& memory);
    ~CPU();

    void reset();
    void irq();
    void nmi();
    void tick();

    const Registers& registers() const { return m_registers; }
    uint8_t cycles() const { return m_cycles; }

private:
    struct Instruction
    {
        bool (CPU::*read_address)(void) = nullptr;
        bool (CPU::*execute)(void) = nullptr;
        std::string mnemonic = "";
        AddressingMode addressing_mode = AddressingMode::AM_IMPLIED;
        uint8_t cycles = 0;
    };

    static Instruction m_instruction_table[256];

    Memory& m_memory;
    Registers m_registers;
    uint8_t m_opcode = 0;
    AddressingMode m_addressing_mode = AM_IMPLIED;
    uint16_t m_address = 0;
    uint8_t m_cycles = 0;

    void interrupt(uint16_t vector);

    void status_set_flag(StatusFlag flag, bool value);
    void status_set_zn_flags(uint8_t value);
    bool status_check_flag(StatusFlag flag) const;

    uint8_t read(uint16_t address);
    uint16_t read_word(uint16_t address);
    void write(uint16_t address, uint8_t data);

    void stack_push(uint8_t data);
    void stack_push_word(uint16_t data);
    uint8_t stack_pop();
    uint16_t stack_pop_word();
    void stack_pop_status();

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

    void branch();

    // Instructions
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
    bool op_axs();
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

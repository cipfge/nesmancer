#include "cpu.hpp"
#include "system_bus.hpp"
#include "logger.hpp"

CPU::Instruction CPU::m_instruction_table[256] = {
    { &CPU::read_implied,          &CPU::op_brk, "BRK", CPU::AM_IMPLIED,            0 }, // Cycle count added by the interrupt
    { &CPU::read_indexed_indirect, &CPU::op_ora, "ORA", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indexed_indirect, &CPU::op_slo, "SLO", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_ora, "ORA", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_asl, "ASL", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_slo, "SLO", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_php, "PHP", CPU::AM_IMPLIED,            3 },
    { &CPU::read_immediate,        &CPU::op_ora, "ORA", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_asl, "ASL", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_anc, "ANC", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_asl, "ASL", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_bpl, "BPL", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_ora, "ORA", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_slo, "SLO", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_ora, "ORA", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_asl, "ASL", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_slo, "SLO", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_clc, "CLC", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_asl, "ASL", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute,         &CPU::op_jsr, "JSR", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_indexed_indirect, &CPU::op_and, "AND", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indexed_indirect, &CPU::op_rla, "RLA", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_bit, "BIT", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_and, "AND", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_rol, "ROL", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_rla, "RLA", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_plp, "PLP", CPU::AM_IMPLIED,            4 },
    { &CPU::read_immediate,        &CPU::op_and, "AND", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_rol, "ROL", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_anc, "ANC", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_bit, "BIT", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_and, "AND", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_rol, "ROL", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_bmi, "BMI", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_and, "AND", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_rla, "RLA", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_and, "AND", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_rol, "ROL", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_rla, "RLA", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_sec, "SEC", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_and, "AND", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_and, "AND", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_rol, "ROL", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_implied,          &CPU::op_rti, "RTI", CPU::AM_IMPLIED,            6 },
    { &CPU::read_indexed_indirect, &CPU::op_eor, "EOR", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indexed_indirect, &CPU::op_sre, "SRE", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_eor, "EOR", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_lsr, "LSR", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_sre, "SRE", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_pha, "PHA", CPU::AM_IMPLIED,            3 },
    { &CPU::read_immediate,        &CPU::op_eor, "EOR", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_lsr, "LSR", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_alr, "ALR", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_jmp, "JMP", CPU::AM_ABSOLUTE,           3 },
    { &CPU::read_absolute,         &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_lsr, "LSR", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_bvc, "BVC", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_eor, "EOR", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_sre, "SRE", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_eor, "EOR", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_lsr, "LSR", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_sre, "SRE", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_cli, "CLI", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_lsr, "LSR", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_implied,          &CPU::op_rts, "RTS", CPU::AM_IMPLIED,            6 },
    { &CPU::read_indexed_indirect, &CPU::op_adc, "ADC", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indexed_indirect, &CPU::op_rra, "RRA", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_adc, "ADC", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_ror, "ROR", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_rra, "RRA", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_pla, "PLA", CPU::AM_IMPLIED,            4 },
    { &CPU::read_immediate,        &CPU::op_adc, "ADC", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_ror, "ROR", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_arr, "ARR", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indirect,         &CPU::op_jmp, "JMP", CPU::AM_INDIRECT,           5 },
    { &CPU::read_absolute,         &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_ror, "ROR", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_bvs, "BVS", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_adc, "ADC", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_rra, "RRA", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_adc, "ADC", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_ror, "ROR", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_rra, "RRA", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_sei, "SEI", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_ror, "ROR", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_sta, "STA", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_sax, "SAX", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_zeropage,         &CPU::op_sty, "STY", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_sta, "STA", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_stx, "STX", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_sax, "SAX", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_implied,          &CPU::op_dey, "DEY", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_txa, "TXA", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_xaa, "XAA", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_sty, "STY", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_sta, "STA", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_stx, "STX", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_sax, "SAX", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_relative,         &CPU::op_bcc, "BCC", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_sta, "STA", CPU::AM_INDIRECT_INDEXED,   6 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_ahx, "AHX", CPU::AM_INDIRECT_INDEXED,   6 },
    { &CPU::read_zeropage_x,       &CPU::op_sty, "STY", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_sta, "STA", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_y,       &CPU::op_stx, "STX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 },
    { &CPU::read_zeropage_y,       &CPU::op_sax, "SAX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_tya, "TYA", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_sta, "STA", CPU::AM_ABSOLUTE_INDEXED_Y, 5 },
    { &CPU::read_implied,          &CPU::op_txs, "TXS", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_tas, "TAS", CPU::AM_ABSOLUTE_INDEXED_Y, 5 },
    { &CPU::read_absolute_x,       &CPU::op_shy, "SHY", CPU::AM_ABSOLUTE_INDEXED_X, 5 },
    { &CPU::read_absolute_x,       &CPU::op_sta, "STA", CPU::AM_ABSOLUTE_INDEXED_X, 5 },
    { &CPU::read_absolute_y,       &CPU::op_shx, "SHX", CPU::AM_ABSOLUTE_INDEXED_Y, 5 },
    { &CPU::read_absolute_y,       &CPU::op_ahx, "AHX", CPU::AM_ABSOLUTE_INDEXED_Y, 5 },
    { &CPU::read_immediate,        &CPU::op_ldy, "LDY", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_lda, "LDA", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_immediate,        &CPU::op_ldx, "LDX", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_lax, "LAX", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_zeropage,         &CPU::op_ldy, "LDY", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_lda, "LDA", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_ldx, "LDX", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_lax, "LAX", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_implied,          &CPU::op_tay, "TAY", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_lda, "LDA", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_tax, "TAX", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_lax, "LAX", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_ldy, "LDY", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_ldx, "LDX", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_lax, "LAX", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_relative,         &CPU::op_bcs, "BCS", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_lda, "LDA", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_lax, "LAX", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_zeropage_x,       &CPU::op_ldy, "LDY", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_lda, "LDA", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_y,       &CPU::op_ldx, "LDX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 },
    { &CPU::read_zeropage_y,       &CPU::op_lax, "LAX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_clv, "CLV", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_tsx, "TSX", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_las, "LAS", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_absolute_x,       &CPU::op_ldy, "LDY", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_y,       &CPU::op_ldx, "LDX", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_absolute_y,       &CPU::op_lax, "LAX", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_immediate,        &CPU::op_cpy, "CPY", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_cmp, "CMP", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_dcp, "DCP", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_cpy, "CPY", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_cmp, "CMP", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_dec, "DEC", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_dcp, "DCP", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_iny, "INY", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_cmp, "CMP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_dex, "DEX", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_axs, "AXS", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_cpy, "CPY", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_dec, "DEC", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_bne, "BNE", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_cmp, "CMP", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_dcp, "DCP", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_cmp, "CMP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_dec, "DEC", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_dcp, "DCP", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_cld, "CLD", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_dec, "DEC", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_immediate,        &CPU::op_cpx, "CPX", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_sbc, "SBC", CPU::AM_INDEXED_INDIRECT,   6 },
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_indexed_indirect, &CPU::op_isc, "ISC", CPU::AM_INDEXED_INDIRECT,   8 },
    { &CPU::read_zeropage,         &CPU::op_cpx, "CPX", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_sbc, "SBC", CPU::AM_ZEROPAGE,           3 },
    { &CPU::read_zeropage,         &CPU::op_inc, "INC", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_zeropage,         &CPU::op_isc, "ISC", CPU::AM_ZEROPAGE,           5 },
    { &CPU::read_implied,          &CPU::op_inx, "INX", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_sbc, "SBC", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_immediate,        &CPU::op_sbc, "SBC", CPU::AM_IMMEDIATE,          2 },
    { &CPU::read_absolute,         &CPU::op_cpx, "CPX", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE,           4 },
    { &CPU::read_absolute,         &CPU::op_inc, "INC", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_absolute,         &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE,           6 },
    { &CPU::read_relative,         &CPU::op_beq, "BEQ", CPU::AM_RELATIVE,           2 },
    { &CPU::read_indirect_indexed, &CPU::op_sbc, "SBC", CPU::AM_INDIRECT_INDEXED,   5 },
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 },
    { &CPU::read_indirect_indexed, &CPU::op_isc, "ISC", CPU::AM_INDIRECT_INDEXED,   8 },
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_sbc, "SBC", CPU::AM_ZEROPAGE_INDEXED_X, 4 },
    { &CPU::read_zeropage_x,       &CPU::op_inc, "INC", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_zeropage_x,       &CPU::op_isc, "ISC", CPU::AM_ZEROPAGE_INDEXED_X, 6 },
    { &CPU::read_implied,          &CPU::op_sed, "SED", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE_INDEXED_Y, 4 },
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 },
    { &CPU::read_absolute_y,       &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE_INDEXED_Y, 7 },
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE_INDEXED_X, 4 },
    { &CPU::read_absolute_x,       &CPU::op_inc, "INC", CPU::AM_ABSOLUTE_INDEXED_X, 7 },
    { &CPU::read_absolute_x,       &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE_INDEXED_X, 7 }
};

void CPU::reset()
{
    m_registers.A = 0;
    m_registers.X = 0;
    m_registers.Y = 0;
    m_registers.P = STATUS_U;
    m_registers.SP = 0xFD;
    m_opcode = 0;
    m_address = 0;
    m_cycles = 0;
    m_dma_cycles = 0;
    m_total_cycles = 0;

    interrupt(InterruptType::RST);
}

void CPU::irq()
{
    interrupt(InterruptType::IRQ);
}

void CPU::nmi()
{
    interrupt(InterruptType::NMI);
}

void CPU::tick()
{
    m_total_cycles++;

    if (m_dma_cycles != 0)
    {
        m_dma_cycles--;
        return;
    }

    if (m_cycles != 0)
    {
        m_cycles--;
        return;
    }

    m_opcode = read(m_registers.PC++);
    Instruction op = m_instruction_table[m_opcode];
    m_addressing_mode = op.addressing_mode;
    m_cycles = op.cycles;
    m_address = 0;
    bool am_cycle = (this->*op.read_address)();
    bool op_cycle = (this->*op.execute)();
    if (am_cycle && op_cycle)
        m_cycles++;

    m_cycles--;
}

void CPU::dma()
{
    // Skip DMA cycles, 256 read + 256 write
    m_dma_cycles = 512;

    // On odd cycles add 1
    if (m_total_cycles & 1)
        m_dma_cycles++;
}

void CPU::interrupt(InterruptType type)
{
    if (type == InterruptType::IRQ && check_status_flag(STATUS_I))
        return;

    if (type != InterruptType::RST)
    {
        if (type == InterruptType::BRK)
        {
            stack_push_word(m_registers.PC++);
            stack_push(m_registers.P | STATUS_B | STATUS_U);
        }
        else
        {
            stack_push_word(m_registers.PC);
            stack_push(m_registers.P | STATUS_U);
        }
    }

    set_status_flag(STATUS_I, true);

    uint16_t vector = IRQ_Vector;
    if (type == InterruptType::RST)
        vector = RST_Vector;
    else if (type == InterruptType::NMI)
        vector = NMI_Vector;

    m_registers.PC = read_word(vector);
    m_cycles = INT_Cycles;
}

void CPU::set_status_zn_flags(uint8_t value)
{
    set_status_flag(STATUS_Z, value == 0);
    set_status_flag(STATUS_N, value >> 7);
}

uint8_t CPU::read(uint16_t address)
{
    return m_system_bus.read(address);
}

uint16_t CPU::read_word(uint16_t address)
{
    return (m_system_bus.read(address) |
            (m_system_bus.read(address + 1) << 8));
}

void CPU::write(uint16_t address, uint8_t data)
{
    m_system_bus.write(address, data);
}

void CPU::stack_push(uint8_t data)
{
    write(0x100 | m_registers.SP, data);
    m_registers.SP--;
}

void CPU::stack_push_word(uint16_t data)
{
    stack_push(data >> 8);
    stack_push(data & 0x00FF);
}

uint8_t CPU::stack_pop()
{
    m_registers.SP++;
    return read(0x100 | m_registers.SP);
}

uint16_t CPU::stack_pop_word()
{
    uint16_t low = static_cast<uint16_t>(stack_pop());
    uint16_t high = static_cast<uint16_t>(stack_pop());

    return (high << 8) | low;
}

void CPU::stack_pop_status()
{
    m_registers.P = stack_pop() & 0xCF;
}

bool CPU::read_implied()
{
    return false;
}

bool CPU::read_immediate()
{
    m_address = m_registers.PC++;
    return false;
}

bool CPU::read_absolute()
{
    m_address = read_word(m_registers.PC);
    m_registers.PC += 2;

    return false;
}

bool CPU::read_absolute_x()
{
    read_absolute();

    const uint16_t base = (m_address & 0xFF00);
    m_address += m_registers.X;

    if (base != (m_address & 0xFF00))
        return true;

    return false;
}

bool CPU::read_absolute_y()
{
    read_absolute();

    const uint16_t base = (m_address & 0xFF00);
    m_address += m_registers.Y;

    if (base != (m_address & 0xFF00))
        return true;

    return false;
}

bool CPU::read_relative()
{
    int8_t offset = read(m_registers.PC++);
    m_address = m_registers.PC + offset;

    if ((m_registers.PC & 0xFF00) != (m_address & 0xFF00))
        return true;

    return false;
}

bool CPU::read_zeropage()
{
    m_address = read(m_registers.PC++) & 0x00FF;
    return false;
}

bool CPU::read_zeropage_x()
{
    m_address = (read(m_registers.PC++) + m_registers.X) & 0x00FF;
    return false;
}

bool CPU::read_zeropage_y()
{
    m_address = (read(m_registers.PC++) + m_registers.Y) & 0x00FF;
    return false;
}

bool CPU::read_indirect()
{
    uint16_t low = read_word(m_registers.PC);
    uint16_t high = (low & 0xFF00) | ((low + 1) & 0x00FF);
    m_registers.PC += 2;
    m_address = static_cast<uint16_t>(read(low)) | static_cast<uint16_t>(read(high)) << 8;

    return false;
}

bool CPU::read_indexed_indirect()
{
    uint16_t low = static_cast<uint16_t>(read(m_registers.PC++) + m_registers.X) & 0x00FF;
    uint16_t high = static_cast<uint16_t>(low + 1) & 0x00FF;
    m_address = static_cast<uint16_t>(read(low)) | static_cast<uint16_t>(read(high)) << 8;

    return false;
}

bool CPU::read_indirect_indexed()
{
    uint16_t low = static_cast<uint16_t>(read(m_registers.PC++));
    uint16_t high = static_cast<uint16_t>(low + 1) & 0x00FF;
    uint16_t address = static_cast<uint16_t>(read(low)) | static_cast<uint16_t>(read(high)) << 8;
    uint16_t base = address & 0xFF00;
    m_address = address + m_registers.Y;

    if (base != (m_address & 0xFF00))
        return true;

    return false;
}

void CPU::branch()
{
    m_registers.PC = m_address;
    m_cycles++;
}

bool CPU::op_bcs()
{
    if (!check_status_flag(STATUS_C))
        return false;

    branch();
    return true;
}

bool CPU::op_bcc()
{
    if (check_status_flag(STATUS_C))
        return false;

    branch();
    return true;
}

bool CPU::op_beq()
{
    if (!check_status_flag(STATUS_Z))
        return false;

    branch();
    return true;
}

bool CPU::op_bne()
{
    if (check_status_flag(STATUS_Z))
        return false;

    branch();
    return true;
}

bool CPU::op_bmi()
{
    if (!check_status_flag(STATUS_N))
        return false;

    branch();
    return true;
}

bool CPU::op_bpl()
{
    if (check_status_flag(STATUS_N))
        return false;

    branch();
    return true;
}

bool CPU::op_bvs()
{
    if (!check_status_flag(STATUS_V))
        return false;

    branch();
    return true;
}

bool CPU::op_bvc()
{
    if (check_status_flag(STATUS_V))
        return false;

    branch();
    return true;
}

bool CPU::op_pha()
{
    stack_push(m_registers.A);
    return false;
}

bool CPU::op_php()
{
    stack_push(m_registers.P | STATUS_B | STATUS_U);
    return false;
}

bool CPU::op_pla()
{
    m_registers.A = stack_pop();
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_plp()
{
    stack_pop_status();
    return false;
}

bool CPU::op_inc()
{
    uint8_t value = read(m_address) + 1;
    write(m_address, value);
    set_status_zn_flags(value);

    return false;
}

bool CPU::op_inx()
{
    m_registers.X++;
    set_status_zn_flags(m_registers.X);

    return false;
}

bool CPU::op_iny()
{
    m_registers.Y++;
    set_status_zn_flags(m_registers.Y);

    return false;
}

bool CPU::op_dec()
{
    uint8_t value = read(m_address) - 1;
    write(m_address, value);
    set_status_zn_flags(value);

    return false;
}

bool CPU::op_dex()
{
    m_registers.X--;
    set_status_zn_flags(m_registers.X);

    return false;
}

bool CPU::op_dey()
{
    m_registers.Y--;
    set_status_zn_flags(m_registers.Y);

    return false;
}

bool CPU::op_adc()
{
    uint8_t operand = read(m_address);
    bool sign = (m_registers.A >> 7) == (operand >> 7);
    uint16_t value = m_registers.A + operand + (check_status_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    bool overflow = sign && (m_registers.A >> 7) != (operand >> 7);

    set_status_flag(STATUS_C, (value & 0x100) >> 8);
    set_status_flag(STATUS_V, overflow);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_sbc()
{
    uint8_t operand = read(m_address) ^ 0xFF;
    bool sign = (m_registers.A & 0x80) == (operand & 0x80);
    uint16_t value = m_registers.A + operand + (check_status_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    bool overflow = sign && (m_registers.A & 0x80) != (operand & 0x80);

    set_status_flag(STATUS_C, (value & 0x100) >> 8);
    set_status_flag(STATUS_V, overflow);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_clc()
{
    set_status_flag(STATUS_C, false);
    return false;
}

bool CPU::op_cld()
{
    set_status_flag(STATUS_D, false);
    return false;
}

bool CPU::op_cli()
{
    set_status_flag(STATUS_I, false);
    return false;
}

bool CPU::op_clv()
{
    set_status_flag(STATUS_V, false);
    return false;
}

bool CPU::op_sec()
{
    set_status_flag(STATUS_C, true);
    return false;
}

bool CPU::op_sed()
{
    set_status_flag(STATUS_D, true);
    return false;
}

bool CPU::op_sei()
{
    set_status_flag(STATUS_I, true);
    return false;
}

bool CPU::op_lda()
{
    m_registers.A = read(m_address);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_ldx()
{
    m_registers.X = read(m_address);
    set_status_zn_flags(m_registers.X);

    return true;
}

bool CPU::op_ldy()
{
    m_registers.Y = read(m_address);
    set_status_zn_flags(m_registers.Y);

    return true;
}

bool CPU::op_sta()
{
    write(m_address, m_registers.A);
    return false;
}

bool CPU::op_stx()
{
    write(m_address, m_registers.X);
    return false;
}

bool CPU::op_sty()
{
    write(m_address, m_registers.Y);
    return false;
}

bool CPU::op_tax()
{
    m_registers.X = m_registers.A;
    set_status_zn_flags(m_registers.X);

    return false;
}

bool CPU::op_tay()
{
    m_registers.Y = m_registers.A;
    set_status_zn_flags(m_registers.Y);

    return false;
}

bool CPU::op_tsx()
{
    m_registers.X = m_registers.SP;
    set_status_zn_flags(m_registers.X);

    return false;
}

bool CPU::op_txa()
{
    m_registers.A = m_registers.X;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_txs()
{
    m_registers.SP = m_registers.X;
    return false;
}

bool CPU::op_tya()
{
    m_registers.A = m_registers.Y;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_jmp()
{
    m_registers.PC = m_address;
    return false;
}

bool CPU::op_rts()
{
    m_registers.PC = stack_pop_word() + 1;
    return false;
}

bool CPU::op_jsr()
{
    stack_push_word(m_registers.PC - 1);
    m_registers.PC = m_address;

    return false;
}

bool CPU::op_brk()
{
    m_registers.PC++;
    interrupt(InterruptType::BRK);
    return false;
}

bool CPU::op_rti()
{
    stack_pop_status();
    m_registers.PC = stack_pop_word();

    return false;
}

bool CPU::op_cmp()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.A - operand;
    set_status_flag(STATUS_C, m_registers.A >= operand);
    set_status_zn_flags(value);

    return true;
}

bool CPU::op_cpx()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.X - operand;
    set_status_flag(STATUS_C, m_registers.X >= operand);
    set_status_zn_flags(value);

    return false;
}

bool CPU::op_cpy()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.Y - operand;
    set_status_flag(STATUS_C, m_registers.Y >= operand);
    set_status_zn_flags(value);

    return false;
}

bool CPU::op_and()
{
    m_registers.A &= read(m_address);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_eor()
{
    m_registers.A ^= read(m_address);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_ora()
{
    m_registers.A |= read(m_address);
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_asl()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        set_status_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A <<= 1;
        set_status_zn_flags(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = operand << 1;
        write(m_address, value);
        set_status_flag(STATUS_C, operand >> 7);
        set_status_zn_flags(value);
    }

    return false;
}

bool CPU::op_lsr()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        set_status_flag(STATUS_C, m_registers.A & 1);
        m_registers.A >>= 1;
        set_status_zn_flags(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = operand >> 1;
        write(m_address, value);
        set_status_flag(STATUS_C, operand & 1);
        set_status_zn_flags(value);
    }

    return false;
}

bool CPU::op_rol()
{
    uint8_t carry = check_status_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        set_status_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A = (m_registers.A << 1) | carry;
        set_status_zn_flags(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = (operand << 1) | carry;
        write(m_address, value);
        set_status_flag(STATUS_C, operand >> 7);
        set_status_zn_flags(value);
    }

    return false;
}

bool CPU::op_ror()
{
    uint8_t carry = check_status_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        set_status_flag(STATUS_C, m_registers.A & 1);
        m_registers.A = (carry << 7) | (m_registers.A >> 1);
        set_status_zn_flags(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = (carry << 7) | (operand >> 1);
        write(m_address, value);
        set_status_flag(STATUS_C, operand & 1);
        set_status_zn_flags(value);
    }

    return false;
}

bool CPU::op_bit()
{
    uint8_t value = read(m_address);

    set_status_flag(STATUS_Z, false);
    set_status_flag(STATUS_V, false);
    set_status_flag(STATUS_N, false);

    if ((m_registers.A & value) == 0)
        set_status_flag(STATUS_Z, true);

    if (value & 0x40)
        set_status_flag(STATUS_V, true);

    if (value & 0x80)
        set_status_flag(STATUS_N, true);

    return false;
}

bool CPU::op_lax()
{
    m_registers.A = read(m_address);
    m_registers.X = m_registers.A;
    set_status_zn_flags(m_registers.A);

    return true;
}

bool CPU::op_sax()
{
    write(m_address, m_registers.A & m_registers.X);
    return false;
}

bool CPU::op_axs()
{
    uint8_t value = read(m_address);
    m_registers.X &= m_registers.A;
    set_status_flag(STATUS_C, m_registers.X >= value);

    m_registers.X -= value;
    set_status_zn_flags(m_registers.X);

    return false;
}

bool CPU::op_dcp()
{
    uint8_t operand = read(m_address) - 1;
    write(m_address, operand);
    uint8_t value = m_registers.A - operand;
    set_status_flag(STATUS_C, m_registers.A >= operand);
    set_status_zn_flags(value);

    return false;
}

bool CPU::op_isc()
{
    uint8_t inc = read(m_address) + 1;
    write(m_address, inc);

    inc ^= 0xFF;
    bool sign = (m_registers.A & 0x80) == (inc & 0x80);
    uint16_t value = m_registers.A + inc + check_status_flag(STATUS_C);
    m_registers.A = value & 0xFF;
    bool overflow = sign && (m_registers.A & 0x80) != (inc & 0x80);

    set_status_flag(STATUS_C, (value & 0x100) >> 8);
    set_status_flag(STATUS_V, overflow);
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_slo()
{
    uint8_t operand = read(m_address);
    uint8_t value = operand << 1;
    set_status_flag(STATUS_C, operand >> 7);
    write(m_address, value);
    m_registers.A |= value;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_rla()
{
    uint8_t operand = read(m_address);
    uint8_t value = (operand << 1) | (check_status_flag(STATUS_C) ? 1 : 0);
    set_status_flag(STATUS_C, operand >> 7);
    write(m_address, value);
    m_registers.A &= value;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_sre()
{
    uint8_t operand = read(m_address);
    uint8_t value = operand >> 1;
    set_status_flag(STATUS_C, operand & 1);
    write(m_address, value);
    m_registers.A ^= value;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_rra()
{
    uint8_t operand = read(m_address);
    uint8_t value = ((check_status_flag(STATUS_C) ? 1 : 0) << 7) | (operand >> 1);
    set_status_flag(STATUS_C, operand & 1);
    write(m_address, value);

    bool sign = (m_registers.A >> 7) == (value >> 7);
    uint16_t result = m_registers.A + value + (check_status_flag(STATUS_C) ? 1 : 0);
    m_registers.A = result & 0xFF;
    bool overflow = sign && (m_registers.A >> 7) != (value >> 7);

    set_status_flag(STATUS_C, (result & 0x100) >> 8);
    set_status_flag(STATUS_V, overflow);
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_anc()
{
    m_registers.A &= read(m_address);
    set_status_flag(STATUS_C, m_registers.A >> 7);
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_alr()
{
    m_registers.A &= read(m_address);
    set_status_flag(STATUS_C, m_registers.A & 1);
    m_registers.A >>= 1;
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_arr()
{
    m_registers.A &= read(m_address);
    m_registers.A = ((check_status_flag(STATUS_C) ? 1 : 0) << 7) | (m_registers.A >> 1);

    bool bit6 = (m_registers.A >> 6) & 1;
    bool bit5 = (m_registers.A >> 5) & 1;

    set_status_flag(STATUS_C, bit6);
    set_status_flag(STATUS_V, bit5 ^ bit6);
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_xaa()
{
    m_registers.A = m_registers.X;
    m_registers.A &= read(m_address);
    set_status_zn_flags(m_registers.A);

    return false;
}

bool CPU::op_las()
{
    uint8_t value = m_registers.SP & read(m_address);
    m_registers.A = value;
    m_registers.X = value;
    m_registers.SP = value;
    set_status_zn_flags(value);

    return true;
}

bool CPU::op_ahx()
{
    write(m_address, ((m_address >> 8) + 1) & m_registers.A & m_registers.X);
    return false;
}

bool CPU::op_tas()
{
    m_registers.SP = m_registers.X & m_registers.A;
    write(m_address, m_registers.SP & ((m_address >> 8) + 1));

    return false;
}

bool CPU::op_shy()
{
    uint8_t low = m_address & 0x00FF;
    uint8_t high = m_address >> 8;
    uint8_t data = m_registers.Y & (high + 1);

    write(((m_registers.Y & (high + 1)) << 8) | low, data);

    return false;
}

bool CPU::op_shx()
{
    uint8_t low = m_address & 0x00FF;
    uint8_t high = m_address >> 8;
    uint8_t data = m_registers.X & (high + 1);

    write(((m_registers.X & (high + 1)) << 8) | low, data);

    return false;
}

bool CPU::op_nop()
{
    return true;
}

bool CPU::op_hlt()
{
    LOG_ERROR("CPU: halt! opcode: %02X", m_opcode);
    return false;
}

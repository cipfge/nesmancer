#include "cpu.hpp"
#include "memory.hpp"
#include <iostream>

CPU::Opcode CPU::m_opcode_table[256] = {
    { "BRK", CPU::AM_IMPLIED,            &CPU::op_brk, &CPU::am_implied,          7 }, // 0x0
    { "ORA", CPU::AM_INDEXED_INDIRECT,   &CPU::op_ora, &CPU::am_indexed_indirect, 6 }, // 0x1
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x2
    { "SLO", CPU::AM_INDEXED_INDIRECT,   &CPU::op_slo, &CPU::am_indexed_indirect, 8 }, // 0x3
    { "NOP", CPU::AM_ZEROPAGE,           &CPU::op_nop, &CPU::am_zeropage,         3 }, // 0x4
    { "ORA", CPU::AM_ZEROPAGE,           &CPU::op_ora, &CPU::am_zeropage,         3 }, // 0x5
    { "ASL", CPU::AM_ZEROPAGE,           &CPU::op_asl, &CPU::am_zeropage,         5 }, // 0x6
    { "SLO", CPU::AM_ZEROPAGE,           &CPU::op_slo, &CPU::am_zeropage,         5 }, // 0x7
    { "PHP", CPU::AM_IMPLIED,            &CPU::op_php, &CPU::am_implied,          3 }, // 0x8
    { "ORA", CPU::AM_IMMEDIATE,          &CPU::op_ora, &CPU::am_immediate,        2 }, // 0x9
    { "ASL", CPU::AM_IMPLIED,            &CPU::op_asl, &CPU::am_implied,          2 }, // 0xA
    { "ANC", CPU::AM_IMMEDIATE,          &CPU::op_anc, &CPU::am_immediate,        2 }, // 0xB
    { "NOP", CPU::AM_ABSOLUTE,           &CPU::op_nop, &CPU::am_absolute,         4 }, // 0xC
    { "ORA", CPU::AM_ABSOLUTE,           &CPU::op_ora, &CPU::am_absolute,         4 }, // 0xD
    { "ASL", CPU::AM_ABSOLUTE,           &CPU::op_asl, &CPU::am_absolute,         6 }, // 0xE
    { "SLO", CPU::AM_ABSOLUTE,           &CPU::op_slo, &CPU::am_absolute,         6 }, // 0xF
    { "BPL", CPU::AM_RELATIVE,           &CPU::op_bpl, &CPU::am_relative,         2 }, // 0x10
    { "ORA", CPU::AM_INDIRECT_INDEXED,   &CPU::op_ora, &CPU::am_indirect_indexed, 5 }, // 0x11
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x12
    { "SLO", CPU::AM_INDIRECT_INDEXED,   &CPU::op_slo, &CPU::am_indirect_indexed, 8 }, // 0x13
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0x14
    { "ORA", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_ora, &CPU::am_zeropage_x,       4 }, // 0x15
    { "ASL", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_asl, &CPU::am_zeropage_x,       6 }, // 0x16
    { "SLO", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_slo, &CPU::am_zeropage_x,       6 }, // 0x17
    { "CLC", CPU::AM_IMPLIED,            &CPU::op_clc, &CPU::am_implied,          2 }, // 0x18
    { "ORA", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_ora, &CPU::am_absolute_y,       4 }, // 0x19
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0x1A
    { "SLO", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_slo, &CPU::am_absolute_y,       7 }, // 0x1B
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0x1C
    { "ORA", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_ora, &CPU::am_absolute_x,       4 }, // 0x1D
    { "ASL", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_asl, &CPU::am_absolute_x,       7 }, // 0x1E
    { "SLO", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_slo, &CPU::am_absolute_x,       7 }, // 0x1F
    { "JSR", CPU::AM_ABSOLUTE,           &CPU::op_jsr, &CPU::am_absolute,         6 }, // 0x20
    { "AND", CPU::AM_INDEXED_INDIRECT,   &CPU::op_and, &CPU::am_indexed_indirect, 6 }, // 0x21
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x22
    { "RLA", CPU::AM_INDEXED_INDIRECT,   &CPU::op_rla, &CPU::am_indexed_indirect, 8 }, // 0x23
    { "BIT", CPU::AM_ZEROPAGE,           &CPU::op_bit, &CPU::am_zeropage,         3 }, // 0x24
    { "AND", CPU::AM_ZEROPAGE,           &CPU::op_and, &CPU::am_zeropage,         3 }, // 0x25
    { "ROL", CPU::AM_ZEROPAGE,           &CPU::op_rol, &CPU::am_zeropage,         5 }, // 0x26
    { "RLA", CPU::AM_ZEROPAGE,           &CPU::op_rla, &CPU::am_zeropage,         5 }, // 0x27
    { "PLP", CPU::AM_IMPLIED,            &CPU::op_plp, &CPU::am_implied,          4 }, // 0x28
    { "AND", CPU::AM_IMMEDIATE,          &CPU::op_and, &CPU::am_immediate,        2 }, // 0x29
    { "ROL", CPU::AM_IMPLIED,            &CPU::op_rol, &CPU::am_implied,          2 }, // 0x2A
    { "ANC", CPU::AM_IMMEDIATE,          &CPU::op_anc, &CPU::am_immediate,        2 }, // 0x2B
    { "BIT", CPU::AM_ABSOLUTE,           &CPU::op_bit, &CPU::am_absolute,         4 }, // 0x2C
    { "AND", CPU::AM_ABSOLUTE,           &CPU::op_and, &CPU::am_absolute,         4 }, // 0x2D
    { "ROL", CPU::AM_ABSOLUTE,           &CPU::op_rol, &CPU::am_absolute,         6 }, // 0x2E
    { "RLA", CPU::AM_ABSOLUTE,           &CPU::op_rla, &CPU::am_absolute,         6 }, // 0x2F
    { "BMI", CPU::AM_RELATIVE,           &CPU::op_bmi, &CPU::am_relative,         2 }, // 0x30
    { "AND", CPU::AM_INDIRECT_INDEXED,   &CPU::op_and, &CPU::am_indirect_indexed, 5 }, // 0x31
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x32
    { "RLA", CPU::AM_INDIRECT_INDEXED,   &CPU::op_rla, &CPU::am_indirect_indexed, 8 }, // 0x33
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0x34
    { "AND", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_and, &CPU::am_zeropage_x,       4 }, // 0x35
    { "ROL", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_rol, &CPU::am_zeropage_x,       6 }, // 0x36
    { "RLA", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_rla, &CPU::am_zeropage_x,       6 }, // 0x37
    { "SEC", CPU::AM_IMPLIED,            &CPU::op_sec, &CPU::am_implied,          2 }, // 0x38
    { "AND", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_and, &CPU::am_absolute_y,       4 }, // 0x39
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0x3A
    { "RLA", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_rla, &CPU::am_absolute_y,       7 }, // 0x3B
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0x3C
    { "AND", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_and, &CPU::am_absolute_x,       4 }, // 0x3D
    { "ROL", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_rol, &CPU::am_absolute_x,       7 }, // 0x3E
    { "RLA", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_rla, &CPU::am_absolute_x,       7 }, // 0x3F
    { "RTI", CPU::AM_IMPLIED,            &CPU::op_rti, &CPU::am_implied,          6 }, // 0x40
    { "EOR", CPU::AM_INDEXED_INDIRECT,   &CPU::op_eor, &CPU::am_indexed_indirect, 6 }, // 0x41
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x42
    { "SRE", CPU::AM_INDEXED_INDIRECT,   &CPU::op_sre, &CPU::am_indexed_indirect, 8 }, // 0x43
    { "NOP", CPU::AM_ZEROPAGE,           &CPU::op_nop, &CPU::am_zeropage,         3 }, // 0x44
    { "EOR", CPU::AM_ZEROPAGE,           &CPU::op_eor, &CPU::am_zeropage,         3 }, // 0x45
    { "LSR", CPU::AM_ZEROPAGE,           &CPU::op_lsr, &CPU::am_zeropage,         5 }, // 0x46
    { "SRE", CPU::AM_ZEROPAGE,           &CPU::op_sre, &CPU::am_zeropage,         5 }, // 0x47
    { "PHA", CPU::AM_IMPLIED,            &CPU::op_pha, &CPU::am_implied,          3 }, // 0x48
    { "EOR", CPU::AM_IMMEDIATE,          &CPU::op_eor, &CPU::am_immediate,        2 }, // 0x49
    { "LSR", CPU::AM_IMPLIED,            &CPU::op_lsr, &CPU::am_implied,          2 }, // 0x4A
    { "ALR", CPU::AM_IMMEDIATE,          &CPU::op_alr, &CPU::am_immediate,        2 }, // 0x4B
    { "JMP", CPU::AM_ABSOLUTE,           &CPU::op_jmp, &CPU::am_absolute,         3 }, // 0x4C
    { "EOR", CPU::AM_ABSOLUTE,           &CPU::op_eor, &CPU::am_absolute,         4 }, // 0x4D
    { "LSR", CPU::AM_ABSOLUTE,           &CPU::op_lsr, &CPU::am_absolute,         6 }, // 0x4E
    { "SRE", CPU::AM_ABSOLUTE,           &CPU::op_sre, &CPU::am_absolute,         6 }, // 0x4F
    { "BVC", CPU::AM_RELATIVE,           &CPU::op_bvc, &CPU::am_relative,         2 }, // 0x50
    { "EOR", CPU::AM_INDIRECT_INDEXED,   &CPU::op_eor, &CPU::am_indirect_indexed, 5 }, // 0x51
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x52
    { "SRE", CPU::AM_INDIRECT_INDEXED,   &CPU::op_sre, &CPU::am_indirect_indexed, 8 }, // 0x53
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0x54
    { "EOR", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_eor, &CPU::am_zeropage_x,       4 }, // 0x55
    { "LSR", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_lsr, &CPU::am_zeropage_x,       6 }, // 0x56
    { "SRE", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_sre, &CPU::am_zeropage_x,       6 }, // 0x57
    { "CLI", CPU::AM_IMPLIED,            &CPU::op_cli, &CPU::am_implied,          2 }, // 0x58
    { "EOR", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_eor, &CPU::am_absolute_y,       4 }, // 0x59
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0x5A
    { "SRE", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_sre, &CPU::am_absolute_y,       7 }, // 0x5B
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0x5C
    { "EOR", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_eor, &CPU::am_absolute_x,       4 }, // 0x5D
    { "LSR", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_lsr, &CPU::am_absolute_x,       7 }, // 0x5E
    { "SRE", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_sre, &CPU::am_absolute_x,       7 }, // 0x5F
    { "RTS", CPU::AM_IMPLIED,            &CPU::op_rts, &CPU::am_implied,          6 }, // 0x60
    { "ADC", CPU::AM_INDEXED_INDIRECT,   &CPU::op_adc, &CPU::am_indexed_indirect, 6 }, // 0x61
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x62
    { "RRA", CPU::AM_INDEXED_INDIRECT,   &CPU::op_rra, &CPU::am_indexed_indirect, 8 }, // 0x63
    { "NOP", CPU::AM_ZEROPAGE,           &CPU::op_nop, &CPU::am_zeropage,         3 }, // 0x64
    { "ADC", CPU::AM_ZEROPAGE,           &CPU::op_adc, &CPU::am_zeropage,         3 }, // 0x65
    { "ROR", CPU::AM_ZEROPAGE,           &CPU::op_ror, &CPU::am_zeropage,         5 }, // 0x66
    { "RRA", CPU::AM_ZEROPAGE,           &CPU::op_rra, &CPU::am_zeropage,         5 }, // 0x67
    { "PLA", CPU::AM_IMPLIED,            &CPU::op_pla, &CPU::am_implied,          4 }, // 0x68
    { "ADC", CPU::AM_IMMEDIATE,          &CPU::op_adc, &CPU::am_immediate,        2 }, // 0x69
    { "ROR", CPU::AM_IMPLIED,            &CPU::op_ror, &CPU::am_implied,          2 }, // 0x6A
    { "ARR", CPU::AM_IMMEDIATE,          &CPU::op_arr, &CPU::am_immediate,        2 }, // 0x6B
    { "JMP", CPU::AM_INDIRECT,           &CPU::op_jmp, &CPU::am_indirect,         5 }, // 0x6C
    { "ADC", CPU::AM_ABSOLUTE,           &CPU::op_adc, &CPU::am_absolute,         4 }, // 0x6D
    { "ROR", CPU::AM_ABSOLUTE,           &CPU::op_ror, &CPU::am_absolute,         6 }, // 0x6E
    { "RRA", CPU::AM_ABSOLUTE,           &CPU::op_rra, &CPU::am_absolute,         6 }, // 0x6F
    { "BVS", CPU::AM_RELATIVE,           &CPU::op_bvs, &CPU::am_relative,         2 }, // 0x70
    { "ADC", CPU::AM_INDIRECT_INDEXED,   &CPU::op_adc, &CPU::am_indirect_indexed, 5 }, // 0x71
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x72
    { "RRA", CPU::AM_INDIRECT_INDEXED,   &CPU::op_rra, &CPU::am_indirect_indexed, 8 }, // 0x73
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0x74
    { "ADC", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_adc, &CPU::am_zeropage_x,       4 }, // 0x75
    { "ROR", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_ror, &CPU::am_zeropage_x,       6 }, // 0x76
    { "RRA", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_rra, &CPU::am_zeropage_x,       6 }, // 0x77
    { "SEI", CPU::AM_IMPLIED,            &CPU::op_sei, &CPU::am_implied,          2 }, // 0x78
    { "ADC", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_adc, &CPU::am_absolute_y,       4 }, // 0x79
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0x7A
    { "RRA", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_rra, &CPU::am_absolute_y,       7 }, // 0x7B
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0x7C
    { "ADC", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_adc, &CPU::am_absolute_x,       4 }, // 0x7D
    { "ROR", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_ror, &CPU::am_absolute_x,       7 }, // 0x7E
    { "RRA", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_rra, &CPU::am_absolute_x,       7 }, // 0x7F
    { "NOP", CPU::AM_IMMEDIATE,          &CPU::op_nop, &CPU::am_immediate,        2 }, // 0x80
    { "STA", CPU::AM_INDEXED_INDIRECT,   &CPU::op_sta, &CPU::am_indexed_indirect, 6 }, // 0x81
    { "NOP", CPU::AM_IMMEDIATE,          &CPU::op_nop, &CPU::am_immediate,        2 }, // 0x82
    { "SAX", CPU::AM_INDEXED_INDIRECT,   &CPU::op_sax, &CPU::am_indexed_indirect, 6 }, // 0x83
    { "STY", CPU::AM_ZEROPAGE,           &CPU::op_sty, &CPU::am_zeropage,         3 }, // 0x84
    { "STA", CPU::AM_ZEROPAGE,           &CPU::op_sta, &CPU::am_zeropage,         3 }, // 0x85
    { "STX", CPU::AM_ZEROPAGE,           &CPU::op_stx, &CPU::am_zeropage,         3 }, // 0x86
    { "SAX", CPU::AM_ZEROPAGE,           &CPU::op_sax, &CPU::am_zeropage,         3 }, // 0x87
    { "DEY", CPU::AM_IMPLIED,            &CPU::op_dey, &CPU::am_implied,          2 }, // 0x88
    { "NOP", CPU::AM_IMMEDIATE,          &CPU::op_nop, &CPU::am_immediate,        2 }, // 0x89
    { "TXA", CPU::AM_IMPLIED,            &CPU::op_txa, &CPU::am_implied,          2 }, // 0x8A
    { "XAA", CPU::AM_IMMEDIATE,          &CPU::op_xaa, &CPU::am_immediate,        2 }, // 0x8B
    { "STY", CPU::AM_ABSOLUTE,           &CPU::op_sty, &CPU::am_absolute,         4 }, // 0x8C
    { "STA", CPU::AM_ABSOLUTE,           &CPU::op_sta, &CPU::am_absolute,         4 }, // 0x8D
    { "STX", CPU::AM_ABSOLUTE,           &CPU::op_stx, &CPU::am_absolute,         4 }, // 0x8E
    { "SAX", CPU::AM_ABSOLUTE,           &CPU::op_sax, &CPU::am_absolute,         4 }, // 0x8F
    { "BCC", CPU::AM_RELATIVE,           &CPU::op_bcc, &CPU::am_relative,         2 }, // 0x90
    { "STA", CPU::AM_INDIRECT_INDEXED,   &CPU::op_sta, &CPU::am_indirect_indexed, 6 }, // 0x91
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0x92
    { "AHX", CPU::AM_INDIRECT_INDEXED,   &CPU::op_ahx, &CPU::am_indirect_indexed, 6 }, // 0x93
    { "STY", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_sty, &CPU::am_zeropage_x,       4 }, // 0x94
    { "STA", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_sta, &CPU::am_zeropage_x,       4 }, // 0x95
    { "STX", CPU::AM_ZEROPAGE_INDEXED_Y, &CPU::op_stx, &CPU::am_zeropage_y,       4 }, // 0x96
    { "SAX", CPU::AM_ZEROPAGE_INDEXED_Y, &CPU::op_sax, &CPU::am_zeropage_y,       4 }, // 0x97
    { "TYA", CPU::AM_IMPLIED,            &CPU::op_tya, &CPU::am_implied,          2 }, // 0x98
    { "STA", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_sta, &CPU::am_absolute_y,       5 }, // 0x99
    { "TXS", CPU::AM_IMPLIED,            &CPU::op_txs, &CPU::am_implied,          2 }, // 0x9A
    { "TAS", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_tas, &CPU::am_absolute_y,       5 }, // 0x9B
    { "SHY", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_shy, &CPU::am_absolute_x,       5 }, // 0x9C
    { "STA", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_sta, &CPU::am_absolute_x,       5 }, // 0x9D
    { "SHX", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_shx, &CPU::am_absolute_y,       5 }, // 0x9E
    { "AHX", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_ahx, &CPU::am_absolute_y,       5 }, // 0x9F
    { "LDY", CPU::AM_IMMEDIATE,          &CPU::op_ldy, &CPU::am_immediate,        2 }, // 0xA0
    { "LDA", CPU::AM_INDEXED_INDIRECT,   &CPU::op_lda, &CPU::am_indexed_indirect, 6 }, // 0xA1
    { "LDX", CPU::AM_IMMEDIATE,          &CPU::op_ldx, &CPU::am_immediate,        2 }, // 0xA2
    { "LAX", CPU::AM_INDEXED_INDIRECT,   &CPU::op_lax, &CPU::am_indexed_indirect, 6 }, // 0xA3
    { "LDY", CPU::AM_ZEROPAGE,           &CPU::op_ldy, &CPU::am_zeropage,         3 }, // 0xA4
    { "LDA", CPU::AM_ZEROPAGE,           &CPU::op_lda, &CPU::am_zeropage,         3 }, // 0xA5
    { "LDX", CPU::AM_ZEROPAGE,           &CPU::op_ldx, &CPU::am_zeropage,         3 }, // 0xA6
    { "LAX", CPU::AM_ZEROPAGE,           &CPU::op_lax, &CPU::am_zeropage,         3 }, // 0xA7
    { "TAY", CPU::AM_IMPLIED,            &CPU::op_tay, &CPU::am_implied,          2 }, // 0xA8
    { "LDA", CPU::AM_IMMEDIATE,          &CPU::op_lda, &CPU::am_immediate,        2 }, // 0xA9
    { "TAX", CPU::AM_IMPLIED,            &CPU::op_tax, &CPU::am_implied,          2 }, // 0xAA
    { "LAX", CPU::AM_IMMEDIATE,          &CPU::op_lax, &CPU::am_immediate,        2 }, // 0xAB
    { "LDY", CPU::AM_ABSOLUTE,           &CPU::op_ldy, &CPU::am_absolute,         4 }, // 0xAC
    { "LDA", CPU::AM_ABSOLUTE,           &CPU::op_lda, &CPU::am_absolute,         4 }, // 0xAD
    { "LDX", CPU::AM_ABSOLUTE,           &CPU::op_ldx, &CPU::am_absolute,         4 }, // 0xAE
    { "LAX", CPU::AM_ABSOLUTE,           &CPU::op_lax, &CPU::am_absolute,         4 }, // 0xAF
    { "BCS", CPU::AM_RELATIVE,           &CPU::op_bcs, &CPU::am_relative,         2 }, // 0xB0
    { "LDA", CPU::AM_INDIRECT_INDEXED,   &CPU::op_lda, &CPU::am_indirect_indexed, 5 }, // 0xB1
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0xB2
    { "LAX", CPU::AM_INDIRECT_INDEXED,   &CPU::op_lax, &CPU::am_indirect_indexed, 5 }, // 0xB3
    { "LDY", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_ldy, &CPU::am_zeropage_x,       4 }, // 0xB4
    { "LDA", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_lda, &CPU::am_zeropage_x,       4 }, // 0xB5
    { "LDX", CPU::AM_ZEROPAGE_INDEXED_Y, &CPU::op_ldx, &CPU::am_zeropage_y,       4 }, // 0xB6
    { "LAX", CPU::AM_ZEROPAGE_INDEXED_Y, &CPU::op_lax, &CPU::am_zeropage_y,       4 }, // 0xB7
    { "CLV", CPU::AM_IMPLIED,            &CPU::op_clv, &CPU::am_implied,          2 }, // 0xB8
    { "LDA", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_lda, &CPU::am_absolute_y,       4 }, // 0xB9
    { "TSX", CPU::AM_IMPLIED,            &CPU::op_tsx, &CPU::am_implied,          2 }, // 0xBA
    { "LAS", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_las, &CPU::am_absolute_y,       4 }, // 0xBB
    { "LDY", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_ldy, &CPU::am_absolute_x,       4 }, // 0xBC
    { "LDA", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_lda, &CPU::am_absolute_x,       4 }, // 0xBD
    { "LDX", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_ldx, &CPU::am_absolute_y,       4 }, // 0xBE
    { "LAX", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_lax, &CPU::am_absolute_y,       4 }, // 0xBF
    { "CPY", CPU::AM_IMMEDIATE,          &CPU::op_cpy, &CPU::am_immediate,        2 }, // 0xC0
    { "CMP", CPU::AM_INDEXED_INDIRECT,   &CPU::op_cmp, &CPU::am_indexed_indirect, 6 }, // 0xC1
    { "NOP", CPU::AM_IMMEDIATE,          &CPU::op_nop, &CPU::am_immediate,        2 }, // 0xC2
    { "DCP", CPU::AM_INDEXED_INDIRECT,   &CPU::op_dcp, &CPU::am_indexed_indirect, 8 }, // 0xC3
    { "CPY", CPU::AM_ZEROPAGE,           &CPU::op_cpy, &CPU::am_zeropage,         3 }, // 0xC4
    { "CMP", CPU::AM_ZEROPAGE,           &CPU::op_cmp, &CPU::am_zeropage,         3 }, // 0xC5
    { "DEC", CPU::AM_ZEROPAGE,           &CPU::op_dec, &CPU::am_zeropage,         5 }, // 0xC6
    { "DCP", CPU::AM_ZEROPAGE,           &CPU::op_dcp, &CPU::am_zeropage,         5 }, // 0xC7
    { "INY", CPU::AM_IMPLIED,            &CPU::op_iny, &CPU::am_implied,          2 }, // 0xC8
    { "CMP", CPU::AM_IMMEDIATE,          &CPU::op_cmp, &CPU::am_immediate,        2 }, // 0xC9
    { "DEX", CPU::AM_IMPLIED,            &CPU::op_dex, &CPU::am_implied,          2 }, // 0xCA
    { "SAX", CPU::AM_IMMEDIATE,          &CPU::op_sax, &CPU::am_immediate,        2 }, // 0xCB
    { "CPY", CPU::AM_ABSOLUTE,           &CPU::op_cpy, &CPU::am_absolute,         4 }, // 0xCC
    { "CMP", CPU::AM_ABSOLUTE,           &CPU::op_cmp, &CPU::am_absolute,         4 }, // 0xCD
    { "DEC", CPU::AM_ABSOLUTE,           &CPU::op_dec, &CPU::am_absolute,         6 }, // 0xCE
    { "DCP", CPU::AM_ABSOLUTE,           &CPU::op_dcp, &CPU::am_absolute,         6 }, // 0xCF
    { "BNE", CPU::AM_RELATIVE,           &CPU::op_bne, &CPU::am_relative,         2 }, // 0xD0
    { "CMP", CPU::AM_INDIRECT_INDEXED,   &CPU::op_cmp, &CPU::am_indirect_indexed, 5 }, // 0xD1
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0xD2
    { "DCP", CPU::AM_INDIRECT_INDEXED,   &CPU::op_dcp, &CPU::am_indirect_indexed, 8 }, // 0xD3
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0xD4
    { "CMP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_cmp, &CPU::am_zeropage_x,       4 }, // 0xD5
    { "DEC", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_dec, &CPU::am_zeropage_x,       6 }, // 0xD6
    { "DCP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_dcp, &CPU::am_zeropage_x,       6 }, // 0xD7
    { "CLD", CPU::AM_IMPLIED,            &CPU::op_cld, &CPU::am_implied,          2 }, // 0xD8
    { "CMP", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_cmp, &CPU::am_absolute_y,       4 }, // 0xD9
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0xDA
    { "DCP", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_dcp, &CPU::am_absolute_y,       7 }, // 0xDB
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0xDC
    { "CMP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_cmp, &CPU::am_absolute_x,       4 }, // 0xDD
    { "DEC", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_dec, &CPU::am_absolute_x,       7 }, // 0xDE
    { "DCP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_dcp, &CPU::am_absolute_x,       7 }, // 0xDF
    { "CPX", CPU::AM_IMMEDIATE,          &CPU::op_cpx, &CPU::am_immediate,        2 }, // 0xE0
    { "SBC", CPU::AM_INDEXED_INDIRECT,   &CPU::op_sbc, &CPU::am_indexed_indirect, 6 }, // 0xE1
    { "NOP", CPU::AM_IMMEDIATE,          &CPU::op_nop, &CPU::am_immediate,        2 }, // 0xE2
    { "ISC", CPU::AM_INDEXED_INDIRECT,   &CPU::op_isc, &CPU::am_indexed_indirect, 8 }, // 0xE3
    { "CPX", CPU::AM_ZEROPAGE,           &CPU::op_cpx, &CPU::am_zeropage,         3 }, // 0xE4
    { "SBC", CPU::AM_ZEROPAGE,           &CPU::op_sbc, &CPU::am_zeropage,         3 }, // 0xE5
    { "INC", CPU::AM_ZEROPAGE,           &CPU::op_inc, &CPU::am_zeropage,         5 }, // 0xE6
    { "ISC", CPU::AM_ZEROPAGE,           &CPU::op_isc, &CPU::am_zeropage,         5 }, // 0xE7
    { "INX", CPU::AM_IMPLIED,            &CPU::op_inx, &CPU::am_implied,          2 }, // 0xE8
    { "SBC", CPU::AM_IMMEDIATE,          &CPU::op_sbc, &CPU::am_immediate,        2 }, // 0xE9
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0xEA
    { "SBC", CPU::AM_IMMEDIATE,          &CPU::op_sbc, &CPU::am_immediate,        2 }, // 0xEB
    { "CPX", CPU::AM_ABSOLUTE,           &CPU::op_cpx, &CPU::am_absolute,         4 }, // 0xEC
    { "SBC", CPU::AM_ABSOLUTE,           &CPU::op_sbc, &CPU::am_absolute,         4 }, // 0xED
    { "INC", CPU::AM_ABSOLUTE,           &CPU::op_inc, &CPU::am_absolute,         6 }, // 0xEE
    { "ISC", CPU::AM_ABSOLUTE,           &CPU::op_isc, &CPU::am_absolute,         6 }, // 0xEF
    { "BEQ", CPU::AM_RELATIVE,           &CPU::op_beq, &CPU::am_relative,         2 }, // 0xF0
    { "SBC", CPU::AM_INDIRECT_INDEXED,   &CPU::op_sbc, &CPU::am_indirect_indexed, 5 }, // 0xF1
    { "HLT", CPU::AM_IMPLIED,            &CPU::op_hlt, &CPU::am_implied,          2 }, // 0xF2
    { "ISC", CPU::AM_INDIRECT_INDEXED,   &CPU::op_isc, &CPU::am_indirect_indexed, 8 }, // 0xF3
    { "NOP", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_nop, &CPU::am_zeropage_x,       4 }, // 0xF4
    { "SBC", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_sbc, &CPU::am_zeropage_x,       4 }, // 0xF5
    { "INC", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_inc, &CPU::am_zeropage_x,       6 }, // 0xF6
    { "ISC", CPU::AM_ZEROPAGE_INDEXED_X, &CPU::op_isc, &CPU::am_zeropage_x,       6 }, // 0xF7
    { "SED", CPU::AM_IMPLIED,            &CPU::op_sed, &CPU::am_implied,          2 }, // 0xF8
    { "SBC", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_sbc, &CPU::am_absolute_y,       4 }, // 0xF9
    { "NOP", CPU::AM_IMPLIED,            &CPU::op_nop, &CPU::am_implied,          2 }, // 0xFA
    { "ISC", CPU::AM_ABSOLUTE_INDEXED_Y, &CPU::op_isc, &CPU::am_absolute_y,       7 }, // 0xFB
    { "NOP", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_nop, &CPU::am_absolute_x,       4 }, // 0xFC
    { "SBC", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_sbc, &CPU::am_absolute_x,       4 }, // 0xFD
    { "INC", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_inc, &CPU::am_absolute_x,       7 }, // 0xFE
    { "ISC", CPU::AM_ABSOLUTE_INDEXED_X, &CPU::op_isc, &CPU::am_absolute_x,       7 }, // 0xFF
};

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

void CPU::irq()
{
    if (check_flag(STATUS_I))
        return;

    Word address;
    address.value = m_registers.PC;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push((m_registers.P | STATUS_U) & ~STATUS_B);
    m_registers.PC = m_memory->read(0xFFFE) | (m_memory->read(0xFFFF) << 8);
    set_flag(STATUS_I, true);
    m_cycles = 7;
}

void CPU::nmi()
{
    Word address;
    address.value = m_registers.PC;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push((m_registers.P | STATUS_U) & ~STATUS_B);
    m_registers.PC = m_memory->read(0xFFFA) | (m_memory->read(0xFFFB) << 8);
    set_flag(STATUS_I, true);
    m_cycles = 7;
}

void CPU::tick()
{
    if (m_cycles > 0)
    {
        m_cycles--;
        return;
    }

    m_opcode = m_memory->read(m_registers.PC++);
    Opcode opcode = m_opcode_table[m_opcode];
    m_cycles = opcode.cycles;
    uint8_t am_cycle = (this->*opcode.read_address)();
    uint8_t op_cycle = (this->*opcode.execute)();
    m_cycles += (am_cycle & op_cycle);
}

void CPU::set_flag(StatusFlag flag, bool value)
{
    m_registers.P = value ? m_registers.P | flag : m_registers.P & (~flag);
}

bool CPU::check_flag(StatusFlag flag)
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

uint8_t CPU::op_asl()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        // Accumulator
        set_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A <<= 1;
        set_flag(STATUS_N, m_registers.A >> 7);
        set_flag(STATUS_Z, m_registers.A == 0);
    }
    else
    {
        // Memory
        uint8_t operand = m_memory->read(m_address);
        uint8_t value = operand << 1;
        set_flag(STATUS_C, operand >> 7);
        set_flag(STATUS_N, value >> 7);
        set_flag(STATUS_Z, value == 0);
        m_memory->write(m_address, value);
    }

    return 0;
}

uint8_t CPU::op_lsr()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        // Accumulator
        set_flag(STATUS_C, m_registers.A & 1);
        m_registers.A >>= 1;
        set_flag(STATUS_N, m_registers.A >> 7);
        set_flag(STATUS_Z, m_registers.A == 0);
    }
    else
    {
        // Memory
        uint8_t operand = m_memory->read(m_address);
        uint8_t value = operand >> 1;
        set_flag(STATUS_C, operand & 1);
        set_flag(STATUS_N, value >> 7);
        set_flag(STATUS_Z, value == 0);
        m_memory->write(m_address, value);
    }

    return 0;
}

uint8_t CPU::op_rol()
{
    uint8_t carry = check_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        // Accumulator
        set_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A = (m_registers.A << 1) | carry;
        set_flag(STATUS_N, m_registers.A >> 7);
        set_flag(STATUS_Z, m_registers.A == 0);
    }
    else
    {
        // Memory
        uint8_t operand = m_memory->read(m_address);
        uint8_t value = (operand << 1) | carry;
        set_flag(STATUS_C, operand >> 7);
        set_flag(STATUS_N, value >> 7);
        set_flag(STATUS_Z, value == 0);
        m_memory->write(m_address, value);
    }

    return 0;
}

uint8_t CPU::op_ror()
{
    uint8_t carry = check_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        // Accumulator
        set_flag(STATUS_C, m_registers.A & 1);
        m_registers.A = (carry << 7) | (m_registers.A >> 1);
        set_flag(STATUS_N, m_registers.A >> 7);
        set_flag(STATUS_Z, m_registers.A == 0);
    }
    else
    {
        // Memory
        uint8_t operand = m_memory->read(m_address);
        uint8_t value = (carry << 7) | (operand >> 1);
        set_flag(STATUS_C, operand & 7);
        set_flag(STATUS_N, value >> 7);
        set_flag(STATUS_Z, value == 0);
        m_memory->write(m_address, value);
    }

    return 0;
}

uint8_t CPU::op_bit()
{
    uint8_t operand = m_memory->read(m_address);
    m_registers.P = (m_registers.P & ~0xC0) | (operand & 0xC0);
    set_flag(STATUS_Z, !(m_registers.A & operand));

    return 0;
}

uint8_t CPU::op_lax()
{
    m_registers.A = m_memory->read(m_address);
    m_registers.X = m_registers.A;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 1;
}

uint8_t CPU::op_sax()
{
    m_memory->write(m_address, m_registers.X);
    return 0;
}

uint8_t CPU::op_dcp()
{
    uint8_t value = m_memory->read(m_address) - 1;
    m_memory->write(m_address, value);
    uint8_t result = m_registers.A - value;
    set_flag(STATUS_C, m_registers.A >= value);
    set_flag(STATUS_N, result >> 7);
    set_flag(STATUS_Z, result == 0);

    return 0;
}

uint8_t CPU::op_isc()
{
    uint8_t value = m_memory->read(m_address) + 1;
    m_memory->write(m_address, value);
    value ^= 0xFF;
    uint8_t sign = (m_registers.A & 0x80) != (value & 0x80);
    uint16_t result = m_registers.A + value + (check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = result & 0xFF;
    uint8_t overflow = sign && (m_registers.A & 0x80) != (value & 0x80);

    set_flag(STATUS_C, (result & 0x100) >> 8);
    set_flag(STATUS_Z, m_registers.A == 0);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_V, overflow);

    return 0;
}

uint8_t CPU::op_slo()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = operand << 1;
    set_flag(STATUS_C, operand >> 7);
    m_memory->write(m_address, value);

    m_registers.A |= value;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_rla()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = (operand << 1) | (check_flag(STATUS_C) ? 1 : 0);
    set_flag(STATUS_C, operand >> 7);
    m_memory->write(m_address, value);

    m_registers.A &= value;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_sre()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = operand >> 1;
    set_flag(STATUS_C, operand & 1);
    m_memory->write(m_address, value);

    m_registers.A ^= value;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_rra()
{
    uint8_t operand = m_memory->read(m_address);
    uint8_t value = ((check_flag(STATUS_C) ? 1 : 0) << 7) | (operand >> 1);
    set_flag(STATUS_C, operand & 1);
    m_memory->write(m_address, value);

    int sign = (m_registers.A >> 7) == (value >> 7);
    uint16_t result = m_registers.A + value + (check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = result & 0xFF;
    uint8_t overflow = sign && (m_registers.A >> 7) != (value >> 7);

    set_flag(STATUS_C, (result & 0x100) >> 8);
    set_flag(STATUS_Z, m_registers.A == 0);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_V, overflow);

    return 1;
}

uint8_t CPU::op_anc()
{
    m_registers.A &= m_memory->read(m_address);
    set_flag(STATUS_C, m_registers.A >> 7);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_alr()
{
    m_registers.A &= m_memory->read(m_address);
    set_flag(STATUS_C, m_registers.A & 1);

    m_registers.A >>= 1;
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_arr()
{
    m_registers.A &= m_memory->read(m_address);
    m_registers.A = ((check_flag(STATUS_C) ? 1 : 0) << 7) | (m_registers.A >> 1);

    bool bit6 = (m_registers.A >> 6) & 1;
    bool bit5 = (m_registers.A >> 5) & 1;

    set_flag(STATUS_C, bit6);
    set_flag(STATUS_V, bit5 ^ bit6);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_xaa()
{
    m_registers.A = m_registers.X;
    m_registers.A &= m_memory->read(m_address);
    set_flag(STATUS_N, m_registers.A >> 7);
    set_flag(STATUS_Z, m_registers.A == 0);

    return 0;
}

uint8_t CPU::op_las()
{
    uint8_t value = m_registers.SP & m_memory->read(m_address);
    m_registers.A = value;
    m_registers.X = value;
    m_registers.SP = value;
    set_flag(STATUS_N, value >> 7);
    set_flag(STATUS_Z, value == 0);

    return 1;
}

uint8_t CPU::op_ahx()
{
    Word address;
    address.value = m_address + 1;
    m_memory->write(m_address, m_registers.A & m_registers.X & address.byte_high);

    return 0;
}

uint8_t CPU::op_tas()
{
    Word address;
    address.value = m_address + 1;
    m_memory->write(m_address, m_registers.A & m_registers.X & address.byte_high);
    m_registers.SP = m_registers.X & m_registers.A;

    return 0;
}

uint8_t CPU::op_shy()
{
    Word address;
    address.value = m_address;
    m_memory->write(m_address, m_registers.Y & (address.byte_high + 1));

    return 0;
}

uint8_t CPU::op_shx()
{
    Word address;
    address.value = m_address;
    m_memory->write(m_address, m_registers.X & (address.byte_high + 1));

    return 0;
}

uint8_t CPU::op_nop()
{
    return 1;
}

uint8_t CPU::op_hlt()
{
    std::cerr << "Opcode: " << (unsigned)m_opcode << ", Halt!\n";
    exit(-1);
    return 0;
}

#include "cpu.hpp"
#include "memory.hpp"
#include "logger.hpp"

CPU::Instruction CPU::m_instruction_table[256] = {
    { &CPU::read_immediate,        &CPU::op_brk, "BRK", CPU::AM_IMMEDIATE,          7 }, // 0x00
    { &CPU::read_indexed_indirect, &CPU::op_ora, "ORA", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x01
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x02
    { &CPU::read_indexed_indirect, &CPU::op_slo, "SLO", CPU::AM_INDEXED_INDIRECT,   8 }, // 0x03
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 }, // 0x04
    { &CPU::read_zeropage,         &CPU::op_ora, "ORA", CPU::AM_ZEROPAGE,           3 }, // 0x05
    { &CPU::read_zeropage,         &CPU::op_asl, "ASL", CPU::AM_ZEROPAGE,           5 }, // 0x06
    { &CPU::read_zeropage,         &CPU::op_slo, "SLO", CPU::AM_ZEROPAGE,           5 }, // 0x07
    { &CPU::read_implied,          &CPU::op_php, "PHP", CPU::AM_IMPLIED,            3 }, // 0x08
    { &CPU::read_immediate,        &CPU::op_ora, "ORA", CPU::AM_IMMEDIATE,          2 }, // 0x09
    { &CPU::read_implied,          &CPU::op_asl, "ASL", CPU::AM_IMPLIED,            2 }, // 0x0A
    { &CPU::read_immediate,        &CPU::op_anc, "ANC", CPU::AM_IMMEDIATE,          2 }, // 0x0B
    { &CPU::read_absolute,         &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE,           4 }, // 0x0C
    { &CPU::read_absolute,         &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE,           4 }, // 0x0D
    { &CPU::read_absolute,         &CPU::op_asl, "ASL", CPU::AM_ABSOLUTE,           6 }, // 0x0E
    { &CPU::read_absolute,         &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE,           6 }, // 0x0F
    { &CPU::read_relative,         &CPU::op_bpl, "BPL", CPU::AM_RELATIVE,           2 }, // 0x10
    { &CPU::read_indirect_indexed, &CPU::op_ora, "ORA", CPU::AM_INDIRECT_INDEXED,   5 }, // 0x11
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x12
    { &CPU::read_indirect_indexed, &CPU::op_slo, "SLO", CPU::AM_INDIRECT_INDEXED,   8 }, // 0x13
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x14
    { &CPU::read_zeropage_x,       &CPU::op_ora, "ORA", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x15
    { &CPU::read_zeropage_x,       &CPU::op_asl, "ASL", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x16
    { &CPU::read_zeropage_x,       &CPU::op_slo, "SLO", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x17
    { &CPU::read_implied,          &CPU::op_clc, "CLC", CPU::AM_IMPLIED,            2 }, // 0x18
    { &CPU::read_absolute_y,       &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0x19
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0x1A
    { &CPU::read_absolute_y,       &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0x1B
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x1C
    { &CPU::read_absolute_x,       &CPU::op_ora, "ORA", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x1D
    { &CPU::read_absolute_x,       &CPU::op_asl, "ASL", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x1E
    { &CPU::read_absolute_x,       &CPU::op_slo, "SLO", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x1F
    { &CPU::read_absolute,         &CPU::op_jsr, "JSR", CPU::AM_ABSOLUTE,           6 }, // 0x20
    { &CPU::read_indexed_indirect, &CPU::op_and, "AND", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x21
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x22
    { &CPU::read_indexed_indirect, &CPU::op_rla, "RLA", CPU::AM_INDEXED_INDIRECT,   8 }, // 0x23
    { &CPU::read_zeropage,         &CPU::op_bit, "BIT", CPU::AM_ZEROPAGE,           3 }, // 0x24
    { &CPU::read_zeropage,         &CPU::op_and, "AND", CPU::AM_ZEROPAGE,           3 }, // 0x25
    { &CPU::read_zeropage,         &CPU::op_rol, "ROL", CPU::AM_ZEROPAGE,           5 }, // 0x26
    { &CPU::read_zeropage,         &CPU::op_rla, "RLA", CPU::AM_ZEROPAGE,           5 }, // 0x27
    { &CPU::read_implied,          &CPU::op_plp, "PLP", CPU::AM_IMPLIED,            4 }, // 0x28
    { &CPU::read_immediate,        &CPU::op_and, "AND", CPU::AM_IMMEDIATE,          2 }, // 0x29
    { &CPU::read_implied,          &CPU::op_rol, "ROL", CPU::AM_IMPLIED,            2 }, // 0x2A
    { &CPU::read_immediate,        &CPU::op_anc, "ANC", CPU::AM_IMMEDIATE,          2 }, // 0x2B
    { &CPU::read_absolute,         &CPU::op_bit, "BIT", CPU::AM_ABSOLUTE,           4 }, // 0x2C
    { &CPU::read_absolute,         &CPU::op_and, "AND", CPU::AM_ABSOLUTE,           4 }, // 0x2D
    { &CPU::read_absolute,         &CPU::op_rol, "ROL", CPU::AM_ABSOLUTE,           6 }, // 0x2E
    { &CPU::read_absolute,         &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE,           6 }, // 0x2F
    { &CPU::read_relative,         &CPU::op_bmi, "BMI", CPU::AM_RELATIVE,           2 }, // 0x30
    { &CPU::read_indirect_indexed, &CPU::op_and, "AND", CPU::AM_INDIRECT_INDEXED,   5 }, // 0x31
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x32
    { &CPU::read_indirect_indexed, &CPU::op_rla, "RLA", CPU::AM_INDIRECT_INDEXED,   8 }, // 0x33
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x34
    { &CPU::read_zeropage_x,       &CPU::op_and, "AND", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x35
    { &CPU::read_zeropage_x,       &CPU::op_rol, "ROL", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x36
    { &CPU::read_zeropage_x,       &CPU::op_rla, "RLA", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x37
    { &CPU::read_implied,          &CPU::op_sec, "SEC", CPU::AM_IMPLIED,            2 }, // 0x38
    { &CPU::read_absolute_y,       &CPU::op_and, "AND", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0x39
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0x3A
    { &CPU::read_absolute_y,       &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0x3B
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x3C
    { &CPU::read_absolute_x,       &CPU::op_and, "AND", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x3D
    { &CPU::read_absolute_x,       &CPU::op_rol, "ROL", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x3E
    { &CPU::read_absolute_x,       &CPU::op_rla, "RLA", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x3F
    { &CPU::read_implied,          &CPU::op_rti, "RTI", CPU::AM_IMPLIED,            6 }, // 0x40
    { &CPU::read_indexed_indirect, &CPU::op_eor, "EOR", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x41
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x42
    { &CPU::read_indexed_indirect, &CPU::op_sre, "SRE", CPU::AM_INDEXED_INDIRECT,   8 }, // 0x43
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 }, // 0x44
    { &CPU::read_zeropage,         &CPU::op_eor, "EOR", CPU::AM_ZEROPAGE,           3 }, // 0x45
    { &CPU::read_zeropage,         &CPU::op_lsr, "LSR", CPU::AM_ZEROPAGE,           5 }, // 0x46
    { &CPU::read_zeropage,         &CPU::op_sre, "SRE", CPU::AM_ZEROPAGE,           5 }, // 0x47
    { &CPU::read_implied,          &CPU::op_pha, "PHA", CPU::AM_IMPLIED,            3 }, // 0x48
    { &CPU::read_immediate,        &CPU::op_eor, "EOR", CPU::AM_IMMEDIATE,          2 }, // 0x49
    { &CPU::read_implied,          &CPU::op_lsr, "LSR", CPU::AM_IMPLIED,            2 }, // 0x4A
    { &CPU::read_immediate,        &CPU::op_alr, "ALR", CPU::AM_IMMEDIATE,          2 }, // 0x4B
    { &CPU::read_absolute,         &CPU::op_jmp, "JMP", CPU::AM_ABSOLUTE,           3 }, // 0x4C
    { &CPU::read_absolute,         &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE,           4 }, // 0x4D
    { &CPU::read_absolute,         &CPU::op_lsr, "LSR", CPU::AM_ABSOLUTE,           6 }, // 0x4E
    { &CPU::read_absolute,         &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE,           6 }, // 0x4F
    { &CPU::read_relative,         &CPU::op_bvc, "BVC", CPU::AM_RELATIVE,           2 }, // 0x50
    { &CPU::read_indirect_indexed, &CPU::op_eor, "EOR", CPU::AM_INDIRECT_INDEXED,   5 }, // 0x51
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x52
    { &CPU::read_indirect_indexed, &CPU::op_sre, "SRE", CPU::AM_INDIRECT_INDEXED,   8 }, // 0x53
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x54
    { &CPU::read_zeropage_x,       &CPU::op_eor, "EOR", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x55
    { &CPU::read_zeropage_x,       &CPU::op_lsr, "LSR", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x56
    { &CPU::read_zeropage_x,       &CPU::op_sre, "SRE", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x57
    { &CPU::read_implied,          &CPU::op_cli, "CLI", CPU::AM_IMPLIED,            2 }, // 0x58
    { &CPU::read_absolute_y,       &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0x59
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0x5A
    { &CPU::read_absolute_y,       &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0x5B
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x5C
    { &CPU::read_absolute_x,       &CPU::op_eor, "EOR", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x5D
    { &CPU::read_absolute_x,       &CPU::op_lsr, "LSR", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x5E
    { &CPU::read_absolute_x,       &CPU::op_sre, "SRE", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x5F
    { &CPU::read_implied,          &CPU::op_rts, "RTS", CPU::AM_IMPLIED,            6 }, // 0x60
    { &CPU::read_indexed_indirect, &CPU::op_adc, "ADC", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x61
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x62
    { &CPU::read_indexed_indirect, &CPU::op_rra, "RRA", CPU::AM_INDEXED_INDIRECT,   8 }, // 0x63
    { &CPU::read_zeropage,         &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE,           3 }, // 0x64
    { &CPU::read_zeropage,         &CPU::op_adc, "ADC", CPU::AM_ZEROPAGE,           3 }, // 0x65
    { &CPU::read_zeropage,         &CPU::op_ror, "ROR", CPU::AM_ZEROPAGE,           5 }, // 0x66
    { &CPU::read_zeropage,         &CPU::op_rra, "RRA", CPU::AM_ZEROPAGE,           5 }, // 0x67
    { &CPU::read_implied,          &CPU::op_pla, "PLA", CPU::AM_IMPLIED,            4 }, // 0x68
    { &CPU::read_immediate,        &CPU::op_adc, "ADC", CPU::AM_IMMEDIATE,          2 }, // 0x69
    { &CPU::read_implied,          &CPU::op_ror, "ROR", CPU::AM_IMPLIED,            2 }, // 0x6A
    { &CPU::read_immediate,        &CPU::op_arr, "ARR", CPU::AM_IMMEDIATE,          2 }, // 0x6B
    { &CPU::read_indirect,         &CPU::op_jmp, "JMP", CPU::AM_INDIRECT,           5 }, // 0x6C
    { &CPU::read_absolute,         &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE,           4 }, // 0x6D
    { &CPU::read_absolute,         &CPU::op_ror, "ROR", CPU::AM_ABSOLUTE,           6 }, // 0x6E
    { &CPU::read_absolute,         &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE,           6 }, // 0x6F
    { &CPU::read_relative,         &CPU::op_bvs, "BVS", CPU::AM_RELATIVE,           2 }, // 0x70
    { &CPU::read_indirect_indexed, &CPU::op_adc, "ADC", CPU::AM_INDIRECT_INDEXED,   5 }, // 0x71
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x72
    { &CPU::read_indirect_indexed, &CPU::op_rra, "RRA", CPU::AM_INDIRECT_INDEXED,   8 }, // 0x73
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x74
    { &CPU::read_zeropage_x,       &CPU::op_adc, "ADC", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x75
    { &CPU::read_zeropage_x,       &CPU::op_ror, "ROR", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x76
    { &CPU::read_zeropage_x,       &CPU::op_rra, "RRA", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0x77
    { &CPU::read_implied,          &CPU::op_sei, "SEI", CPU::AM_IMPLIED,            2 }, // 0x78
    { &CPU::read_absolute_y,       &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0x79
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0x7A
    { &CPU::read_absolute_y,       &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0x7B
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x7C
    { &CPU::read_absolute_x,       &CPU::op_adc, "ADC", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0x7D
    { &CPU::read_absolute_x,       &CPU::op_ror, "ROR", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x7E
    { &CPU::read_absolute_x,       &CPU::op_rra, "RRA", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0x7F
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 }, // 0x80
    { &CPU::read_indexed_indirect, &CPU::op_sta, "STA", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x81
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 }, // 0x82
    { &CPU::read_indexed_indirect, &CPU::op_sax, "SAX", CPU::AM_INDEXED_INDIRECT,   6 }, // 0x83
    { &CPU::read_zeropage,         &CPU::op_sty, "STY", CPU::AM_ZEROPAGE,           3 }, // 0x84
    { &CPU::read_zeropage,         &CPU::op_sta, "STA", CPU::AM_ZEROPAGE,           3 }, // 0x85
    { &CPU::read_zeropage,         &CPU::op_stx, "STX", CPU::AM_ZEROPAGE,           3 }, // 0x86
    { &CPU::read_zeropage,         &CPU::op_sax, "SAX", CPU::AM_ZEROPAGE,           3 }, // 0x87
    { &CPU::read_implied,          &CPU::op_dey, "DEY", CPU::AM_IMPLIED,            2 }, // 0x88
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 }, // 0x89
    { &CPU::read_implied,          &CPU::op_txa, "TXA", CPU::AM_IMPLIED,            2 }, // 0x8A
    { &CPU::read_immediate,        &CPU::op_xaa, "XAA", CPU::AM_IMMEDIATE,          2 }, // 0x8B
    { &CPU::read_absolute,         &CPU::op_sty, "STY", CPU::AM_ABSOLUTE,           4 }, // 0x8C
    { &CPU::read_absolute,         &CPU::op_sta, "STA", CPU::AM_ABSOLUTE,           4 }, // 0x8D
    { &CPU::read_absolute,         &CPU::op_stx, "STX", CPU::AM_ABSOLUTE,           4 }, // 0x8E
    { &CPU::read_absolute,         &CPU::op_sax, "SAX", CPU::AM_ABSOLUTE,           4 }, // 0x8F
    { &CPU::read_relative,         &CPU::op_bcc, "BCC", CPU::AM_RELATIVE,           2 }, // 0x90
    { &CPU::read_indirect_indexed, &CPU::op_sta, "STA", CPU::AM_INDIRECT_INDEXED,   6 }, // 0x91
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0x92
    { &CPU::read_indirect_indexed, &CPU::op_ahx, "AHX", CPU::AM_INDIRECT_INDEXED,   6 }, // 0x93
    { &CPU::read_zeropage_x,       &CPU::op_sty, "STY", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x94
    { &CPU::read_zeropage_x,       &CPU::op_sta, "STA", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0x95
    { &CPU::read_zeropage_y,       &CPU::op_stx, "STX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 }, // 0x96
    { &CPU::read_zeropage_y,       &CPU::op_sax, "SAX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 }, // 0x97
    { &CPU::read_implied,          &CPU::op_tya, "TYA", CPU::AM_IMPLIED,            2 }, // 0x98
    { &CPU::read_absolute_y,       &CPU::op_sta, "STA", CPU::AM_ABSOLUTE_INDEXED_Y, 5 }, // 0x99
    { &CPU::read_implied,          &CPU::op_txs, "TXS", CPU::AM_IMPLIED,            2 }, // 0x9A
    { &CPU::read_absolute_y,       &CPU::op_tas, "TAS", CPU::AM_ABSOLUTE_INDEXED_Y, 5 }, // 0x9B
    { &CPU::read_absolute_x,       &CPU::op_shy, "SHY", CPU::AM_ABSOLUTE_INDEXED_X, 5 }, // 0x9C
    { &CPU::read_absolute_x,       &CPU::op_sta, "STA", CPU::AM_ABSOLUTE_INDEXED_X, 5 }, // 0x9D
    { &CPU::read_absolute_y,       &CPU::op_shx, "SHX", CPU::AM_ABSOLUTE_INDEXED_Y, 5 }, // 0x9E
    { &CPU::read_absolute_y,       &CPU::op_ahx, "AHX", CPU::AM_ABSOLUTE_INDEXED_Y, 5 }, // 0x9F
    { &CPU::read_immediate,        &CPU::op_ldy, "LDY", CPU::AM_IMMEDIATE,          2 }, // 0xA0
    { &CPU::read_indexed_indirect, &CPU::op_lda, "LDA", CPU::AM_INDEXED_INDIRECT,   6 }, // 0xA1
    { &CPU::read_immediate,        &CPU::op_ldx, "LDX", CPU::AM_IMMEDIATE,          2 }, // 0xA2
    { &CPU::read_indexed_indirect, &CPU::op_lax, "LAX", CPU::AM_INDEXED_INDIRECT,   6 }, // 0xA3
    { &CPU::read_zeropage,         &CPU::op_ldy, "LDY", CPU::AM_ZEROPAGE,           3 }, // 0xA4
    { &CPU::read_zeropage,         &CPU::op_lda, "LDA", CPU::AM_ZEROPAGE,           3 }, // 0xA5
    { &CPU::read_zeropage,         &CPU::op_ldx, "LDX", CPU::AM_ZEROPAGE,           3 }, // 0xA6
    { &CPU::read_zeropage,         &CPU::op_lax, "LAX", CPU::AM_ZEROPAGE,           3 }, // 0xA7
    { &CPU::read_implied,          &CPU::op_tay, "TAY", CPU::AM_IMPLIED,            2 }, // 0xA8
    { &CPU::read_immediate,        &CPU::op_lda, "LDA", CPU::AM_IMMEDIATE,          2 }, // 0xA9
    { &CPU::read_implied,          &CPU::op_tax, "TAX", CPU::AM_IMPLIED,            2 }, // 0xAA
    { &CPU::read_immediate,        &CPU::op_lax, "LAX", CPU::AM_IMMEDIATE,          2 }, // 0xAB
    { &CPU::read_absolute,         &CPU::op_ldy, "LDY", CPU::AM_ABSOLUTE,           4 }, // 0xAC
    { &CPU::read_absolute,         &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE,           4 }, // 0xAD
    { &CPU::read_absolute,         &CPU::op_ldx, "LDX", CPU::AM_ABSOLUTE,           4 }, // 0xAE
    { &CPU::read_absolute,         &CPU::op_lax, "LAX", CPU::AM_ABSOLUTE,           4 }, // 0xAF
    { &CPU::read_relative,         &CPU::op_bcs, "BCS", CPU::AM_RELATIVE,           2 }, // 0xB0
    { &CPU::read_indirect_indexed, &CPU::op_lda, "LDA", CPU::AM_INDIRECT_INDEXED,   5 }, // 0xB1
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0xB2
    { &CPU::read_indirect_indexed, &CPU::op_lax, "LAX", CPU::AM_INDIRECT_INDEXED,   5 }, // 0xB3
    { &CPU::read_zeropage_x,       &CPU::op_ldy, "LDY", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xB4
    { &CPU::read_zeropage_x,       &CPU::op_lda, "LDA", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xB5
    { &CPU::read_zeropage_y,       &CPU::op_ldx, "LDX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 }, // 0xB6
    { &CPU::read_zeropage_y,       &CPU::op_lax, "LAX", CPU::AM_ZEROPAGE_INDEXED_Y, 4 }, // 0xB7
    { &CPU::read_implied,          &CPU::op_clv, "CLV", CPU::AM_IMPLIED,            2 }, // 0xB8
    { &CPU::read_absolute_y,       &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xB9
    { &CPU::read_implied,          &CPU::op_tsx, "TSX", CPU::AM_IMPLIED,            2 }, // 0xBA
    { &CPU::read_absolute_y,       &CPU::op_las, "LAS", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xBB
    { &CPU::read_absolute_x,       &CPU::op_ldy, "LDY", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xBC
    { &CPU::read_absolute_x,       &CPU::op_lda, "LDA", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xBD
    { &CPU::read_absolute_y,       &CPU::op_ldx, "LDX", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xBE
    { &CPU::read_absolute_y,       &CPU::op_lax, "LAX", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xBF
    { &CPU::read_immediate,        &CPU::op_cpy, "CPY", CPU::AM_IMMEDIATE,          2 }, // 0xC0
    { &CPU::read_indexed_indirect, &CPU::op_cmp, "CMP", CPU::AM_INDEXED_INDIRECT,   6 }, // 0xC1
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 }, // 0xC2
    { &CPU::read_indexed_indirect, &CPU::op_dcp, "DCP", CPU::AM_INDEXED_INDIRECT,   8 }, // 0xC3
    { &CPU::read_zeropage,         &CPU::op_cpy, "CPY", CPU::AM_ZEROPAGE,           3 }, // 0xC4
    { &CPU::read_zeropage,         &CPU::op_cmp, "CMP", CPU::AM_ZEROPAGE,           3 }, // 0xC5
    { &CPU::read_zeropage,         &CPU::op_dec, "DEC", CPU::AM_ZEROPAGE,           5 }, // 0xC6
    { &CPU::read_zeropage,         &CPU::op_dcp, "DCP", CPU::AM_ZEROPAGE,           5 }, // 0xC7
    { &CPU::read_implied,          &CPU::op_iny, "INY", CPU::AM_IMPLIED,            2 }, // 0xC8
    { &CPU::read_immediate,        &CPU::op_cmp, "CMP", CPU::AM_IMMEDIATE,          2 }, // 0xC9
    { &CPU::read_implied,          &CPU::op_dex, "DEX", CPU::AM_IMPLIED,            2 }, // 0xCA
    { &CPU::read_immediate,        &CPU::op_axs, "AXS", CPU::AM_IMMEDIATE,          2 }, // 0xCB
    { &CPU::read_absolute,         &CPU::op_cpy, "CPY", CPU::AM_ABSOLUTE,           4 }, // 0xCC
    { &CPU::read_absolute,         &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE,           4 }, // 0xCD
    { &CPU::read_absolute,         &CPU::op_dec, "DEC", CPU::AM_ABSOLUTE,           6 }, // 0xCE
    { &CPU::read_absolute,         &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE,           6 }, // 0xCF
    { &CPU::read_relative,         &CPU::op_bne, "BNE", CPU::AM_RELATIVE,           2 }, // 0xD0
    { &CPU::read_indirect_indexed, &CPU::op_cmp, "CMP", CPU::AM_INDIRECT_INDEXED,   5 }, // 0xD1
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0xD2
    { &CPU::read_indirect_indexed, &CPU::op_dcp, "DCP", CPU::AM_INDIRECT_INDEXED,   8 }, // 0xD3
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xD4
    { &CPU::read_zeropage_x,       &CPU::op_cmp, "CMP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xD5
    { &CPU::read_zeropage_x,       &CPU::op_dec, "DEC", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0xD6
    { &CPU::read_zeropage_x,       &CPU::op_dcp, "DCP", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0xD7
    { &CPU::read_implied,          &CPU::op_cld, "CLD", CPU::AM_IMPLIED,            2 }, // 0xD8
    { &CPU::read_absolute_y,       &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xD9
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0xDA
    { &CPU::read_absolute_y,       &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0xDB
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xDC
    { &CPU::read_absolute_x,       &CPU::op_cmp, "CMP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xDD
    { &CPU::read_absolute_x,       &CPU::op_dec, "DEC", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0xDE
    { &CPU::read_absolute_x,       &CPU::op_dcp, "DCP", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0xDF
    { &CPU::read_immediate,        &CPU::op_cpx, "CPX", CPU::AM_IMMEDIATE,          2 }, // 0xE0
    { &CPU::read_indexed_indirect, &CPU::op_sbc, "SBC", CPU::AM_INDEXED_INDIRECT,   6 }, // 0xE1
    { &CPU::read_immediate,        &CPU::op_nop, "NOP", CPU::AM_IMMEDIATE,          2 }, // 0xE2
    { &CPU::read_indexed_indirect, &CPU::op_isc, "ISC", CPU::AM_INDEXED_INDIRECT,   8 }, // 0xE3
    { &CPU::read_zeropage,         &CPU::op_cpx, "CPX", CPU::AM_ZEROPAGE,           3 }, // 0xE4
    { &CPU::read_zeropage,         &CPU::op_sbc, "SBC", CPU::AM_ZEROPAGE,           3 }, // 0xE5
    { &CPU::read_zeropage,         &CPU::op_inc, "INC", CPU::AM_ZEROPAGE,           5 }, // 0xE6
    { &CPU::read_zeropage,         &CPU::op_isc, "ISC", CPU::AM_ZEROPAGE,           5 }, // 0xE7
    { &CPU::read_implied,          &CPU::op_inx, "INX", CPU::AM_IMPLIED,            2 }, // 0xE8
    { &CPU::read_immediate,        &CPU::op_sbc, "SBC", CPU::AM_IMMEDIATE,          2 }, // 0xE9
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0xEA
    { &CPU::read_immediate,        &CPU::op_sbc, "SBC", CPU::AM_IMMEDIATE,          2 }, // 0xEB
    { &CPU::read_absolute,         &CPU::op_cpx, "CPX", CPU::AM_ABSOLUTE,           4 }, // 0xEC
    { &CPU::read_absolute,         &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE,           4 }, // 0xED
    { &CPU::read_absolute,         &CPU::op_inc, "INC", CPU::AM_ABSOLUTE,           6 }, // 0xEE
    { &CPU::read_absolute,         &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE,           6 }, // 0xEF
    { &CPU::read_relative,         &CPU::op_beq, "BEQ", CPU::AM_RELATIVE,           2 }, // 0xF0
    { &CPU::read_indirect_indexed, &CPU::op_sbc, "SBC", CPU::AM_INDIRECT_INDEXED,   5 }, // 0xF1
    { &CPU::read_implied,          &CPU::op_hlt, "HLT", CPU::AM_IMPLIED,            2 }, // 0xF2
    { &CPU::read_indirect_indexed, &CPU::op_isc, "ISC", CPU::AM_INDIRECT_INDEXED,   8 }, // 0xF3
    { &CPU::read_zeropage_x,       &CPU::op_nop, "NOP", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xF4
    { &CPU::read_zeropage_x,       &CPU::op_sbc, "SBC", CPU::AM_ZEROPAGE_INDEXED_X, 4 }, // 0xF5
    { &CPU::read_zeropage_x,       &CPU::op_inc, "INC", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0xF6
    { &CPU::read_zeropage_x,       &CPU::op_isc, "ISC", CPU::AM_ZEROPAGE_INDEXED_X, 6 }, // 0xF7
    { &CPU::read_implied,          &CPU::op_sed, "SED", CPU::AM_IMPLIED,            2 }, // 0xF8
    { &CPU::read_absolute_y,       &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE_INDEXED_Y, 4 }, // 0xF9
    { &CPU::read_implied,          &CPU::op_nop, "NOP", CPU::AM_IMPLIED,            2 }, // 0xFA
    { &CPU::read_absolute_y,       &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE_INDEXED_Y, 7 }, // 0xFB
    { &CPU::read_absolute_x,       &CPU::op_nop, "NOP", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xFC
    { &CPU::read_absolute_x,       &CPU::op_sbc, "SBC", CPU::AM_ABSOLUTE_INDEXED_X, 4 }, // 0xFD
    { &CPU::read_absolute_x,       &CPU::op_inc, "INC", CPU::AM_ABSOLUTE_INDEXED_X, 7 }, // 0xFE
    { &CPU::read_absolute_x,       &CPU::op_isc, "ISC", CPU::AM_ABSOLUTE_INDEXED_X, 7 }  // 0xFF
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
    m_registers.PC = read_word(0xFFFC);
    m_opcode = 0;
    m_address = 0;
    m_cycles = 7;
}

void CPU::irq()
{
    if (status_check_flag(STATUS_I))
        return;

    Word address;
    address.value = m_registers.PC;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push((m_registers.P | STATUS_U) & ~STATUS_B);
    m_registers.PC = read_word(0xFFFE);
    status_set_flag(STATUS_I, true);
    m_cycles = 7;
}

void CPU::nmi()
{
    Word address;
    address.value = m_registers.PC;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push((m_registers.P | STATUS_U) & ~STATUS_B);
    m_registers.PC = read_word(0xFFFA);
    status_set_flag(STATUS_I, true);
    m_cycles = 7;
}

void CPU::tick()
{
    if (m_cycles == 0)
    {
        m_opcode = read(m_registers.PC++);
        Instruction op = m_instruction_table[m_opcode];
        m_addressing_mode = op.addressing_mode;
        m_cycles = op.cycles;
        bool am_cycle = (this->*op.read_address)();
        bool op_cycle = (this->*op.execute)();
        if (am_cycle && op_cycle)
            m_cycles++;
    }

    m_cycles--;
}

inline void CPU::status_set_flag(StatusFlag flag, bool value)
{
    m_registers.P = value ? m_registers.P | flag : m_registers.P & (~flag);
}

inline bool CPU::status_check_flag(StatusFlag flag)
{
    return (m_registers.P & flag) != 0;
}

inline void CPU::status_set_zn(uint8_t value)
{
    status_set_flag(STATUS_Z, value == 0);
    status_set_flag(STATUS_N, value >> 7);
}

inline uint8_t CPU::read(uint16_t address)
{
    return m_memory->read(address);
}

inline uint16_t CPU::read_word(uint16_t address)
{
    return (m_memory->read(address) |
            (m_memory->read(address + 1) << 8));
}

inline void CPU::write(uint16_t address, uint8_t data)
{
    m_memory->write(address, data);
}

inline void CPU::stack_push(uint8_t data)
{
    write(0x100 | m_registers.SP, data);
    m_registers.SP--;
}

inline uint8_t CPU::stack_pop()
{
    m_registers.SP++;
    return read(0x100 | m_registers.SP);
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
    Word address;
    address.byte_low = read(m_registers.PC++);
    address.byte_high = read(m_registers.PC++);
    m_address = address.value;

    return false;
}

bool CPU::read_absolute_x()
{
    Word address;
    address.byte_low = read(m_registers.PC++);
    address.byte_high = read(m_registers.PC++);
    m_address = address.value + m_registers.X;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
        return true;

    return false;
}

bool CPU::read_absolute_y()
{
    Word address;
    address.byte_low = read(m_registers.PC++);
    address.byte_high = read(m_registers.PC++);
    m_address = address.value + m_registers.Y;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
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
    m_address = read(m_registers.PC++);
    m_address &= 0xFF;

    return false;
}

bool CPU::read_zeropage_x()
{
    m_address = read(m_registers.PC++) + m_registers.X;
    m_address &= 0xFF;

    return false;
}

bool CPU::read_zeropage_y()
{
    m_address = read(m_registers.PC++) + m_registers.Y;
    m_address &= 0xFF;

    return false;
}

bool CPU::read_indirect()
{
    Word pointer;
    pointer.byte_low = read(m_registers.PC++);
    pointer.byte_high = read(m_registers.PC++);

    // 6502 page boundary hardware bug
    uint16_t byte_high = pointer.byte_low == 0xFF ? pointer.value & 0xFF00 : pointer.value + 1;

    Word address;
    address.byte_low = read(pointer.value);
    address.byte_high = read(byte_high);
    m_address = address.value;

    return false;
}

bool CPU::read_indexed_indirect()
{
    uint8_t zpg = m_registers.X + read(m_registers.PC++);

    Word address;
    address.byte_low = read(zpg++);
    address.byte_high = read(zpg);
    m_address = address.value;

    return false;
}

bool CPU::read_indirect_indexed()
{
    uint8_t zpg = read(m_registers.PC++);

    Word address;
    address.byte_low = read(zpg++);
    address.byte_high = read(zpg);
    m_address = address.value + m_registers.Y;

    if ((address.value & 0xFF00) != (m_address & 0xFF00))
        return true;

    return false;
}

inline void CPU::branch()
{
    m_registers.PC = m_address;
    m_cycles++;
}

bool CPU::op_bcs()
{
    if (!status_check_flag(STATUS_C))
        return false;

    branch();
    return true;
}

bool CPU::op_bcc()
{
    if (status_check_flag(STATUS_C))
        return false;

    branch();
    return true;
}

bool CPU::op_beq()
{
    if (!status_check_flag(STATUS_Z))
        return false;

    branch();
    return true;
}

bool CPU::op_bne()
{
    if (status_check_flag(STATUS_Z))
        return false;

    branch();
    return true;
}

bool CPU::op_bmi()
{
    if (!status_check_flag(STATUS_N))
        return false;

    branch();
    return true;
}

bool CPU::op_bpl()
{
    if (status_check_flag(STATUS_N))
        return false;

    branch();
    return true;
}

bool CPU::op_bvs()
{
    if (!status_check_flag(STATUS_V))
        return false;

    branch();
    return true;
}

bool CPU::op_bvc()
{
    if (status_check_flag(STATUS_V))
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
    stack_push(m_registers.P | STATUS_U | STATUS_B);
    return false;
}

bool CPU::op_pla()
{
    m_registers.A = stack_pop();
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_plp()
{
    m_registers.P = (stack_pop() | STATUS_U) & ~STATUS_B;
    return false;
}

bool CPU::op_inc()
{
    uint8_t value = read(m_address) + 1;
    write(m_address, value);
    status_set_zn(value);

    return false;
}

bool CPU::op_inx()
{
    m_registers.X++;
    status_set_zn(m_registers.X);

    return false;
}

bool CPU::op_iny()
{
    m_registers.Y++;
    status_set_zn(m_registers.Y);

    return false;
}

bool CPU::op_dec()
{
    uint8_t value = read(m_address) - 1;
    write(m_address, value);
    status_set_zn(value);

    return false;
}

bool CPU::op_dex()
{
    m_registers.X--;
    status_set_zn(m_registers.X);

    return false;
}

bool CPU::op_dey()
{
    m_registers.Y--;
    status_set_zn(m_registers.Y);

    return false;
}

bool CPU::op_adc()
{
    uint8_t operand = read(m_address);
    int sign = (m_registers.A >> 7) == (operand >> 7);
    uint16_t value = m_registers.A + operand + (status_check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    uint8_t overflow = sign && (m_registers.A >> 7) != (operand >> 7);

    status_set_flag(STATUS_C, (value & 0x100) >> 8);
    status_set_flag(STATUS_V, overflow);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_sbc()
{
    uint8_t operand = read(m_address) ^ 0xFF;
    int sign = (m_registers.A & 0x80) == (operand & 0x80);
    uint16_t value = m_registers.A + operand + (status_check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = value & 0xFF;
    uint8_t overflow = sign && (m_registers.A & 0x80) != (operand & 0x80);

    status_set_flag(STATUS_C, (value & 0x100) >> 8);
    status_set_flag(STATUS_V, overflow);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_clc()
{
    status_set_flag(STATUS_C, false);
    return false;
}

bool CPU::op_cld()
{
    status_set_flag(STATUS_D, false);
    return false;
}

bool CPU::op_cli()
{
    status_set_flag(STATUS_I, false);
    return false;
}

bool CPU::op_clv()
{
    status_set_flag(STATUS_V, false);
    return false;
}

bool CPU::op_sec()
{
    status_set_flag(STATUS_C, true);
    return false;
}

bool CPU::op_sed()
{
    status_set_flag(STATUS_D, true);
    return false;
}

bool CPU::op_sei()
{
    status_set_flag(STATUS_I, true);
    return false;
}

bool CPU::op_lda()
{
    m_registers.A = read(m_address);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_ldx()
{
    m_registers.X = read(m_address);
    status_set_zn(m_registers.X);

    return true;
}

bool CPU::op_ldy()
{
    m_registers.Y = read(m_address);
    status_set_zn(m_registers.Y);

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
    status_set_zn(m_registers.X);

    return false;
}

bool CPU::op_tay()
{
    m_registers.Y = m_registers.A;
    status_set_zn(m_registers.Y);

    return false;
}

bool CPU::op_tsx()
{
    m_registers.X = m_registers.SP;
    status_set_zn(m_registers.X);

    return false;
}

bool CPU::op_txa()
{
    m_registers.A = m_registers.X;
    status_set_zn(m_registers.A);

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
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_jmp()
{
    m_registers.PC = m_address;
    return false;
}

bool CPU::op_rts()
{
    Word address;
    address.byte_low = stack_pop();
    address.byte_high = stack_pop();
    m_registers.PC = address.value + 1;

    return false;
}

bool CPU::op_jsr()
{
    Word address;
    address.value = m_registers.PC - 1;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    m_registers.PC = m_address;

    return false;
}

bool CPU::op_brk()
{
    Word address;
    address.value = m_registers.PC + 1;
    stack_push(address.byte_high);
    stack_push(address.byte_low);
    stack_push(m_registers.P | STATUS_U | STATUS_B);
    m_registers.PC = read_word(0xFFFE);
    status_set_flag(STATUS_I, true);

    return false;
}

bool CPU::op_rti()
{
    m_registers.P = (stack_pop() | STATUS_U) & ~STATUS_B;

    Word address;
    address.byte_low = stack_pop();
    address.byte_high = stack_pop();
    m_registers.PC = address.value;

    return false;
}

bool CPU::op_cmp()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.A - operand;
    status_set_flag(STATUS_C, m_registers.A >= operand);
    status_set_zn(value);

    return true;
}

bool CPU::op_cpx()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.X - operand;
    status_set_flag(STATUS_C, m_registers.X >= operand);
    status_set_zn(value);

    return false;
}

bool CPU::op_cpy()
{
    uint8_t operand = read(m_address);
    uint8_t value = m_registers.Y - operand;
    status_set_flag(STATUS_C, m_registers.Y >= operand);
    status_set_zn(value);

    return false;
}

bool CPU::op_and()
{
    m_registers.A &= read(m_address);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_eor()
{
    m_registers.A ^= read(m_address);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_ora()
{
    m_registers.A |= read(m_address);
    status_set_zn(m_registers.A);

    return true;
}

bool CPU::op_asl()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        status_set_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A <<= 1;
        status_set_zn(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = operand << 1;
        write(m_address, value);
        status_set_flag(STATUS_C, operand >> 7);
        status_set_zn(value);
    }

    return false;
}

bool CPU::op_lsr()
{
    if (m_addressing_mode == AM_IMPLIED)
    {
        status_set_flag(STATUS_C, m_registers.A & 1);
        m_registers.A >>= 1;
        status_set_zn(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = operand >> 1;
        write(m_address, value);
        status_set_flag(STATUS_C, operand & 1);
        status_set_zn(value);
    }

    return false;
}

bool CPU::op_rol()
{
    uint8_t carry = status_check_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        status_set_flag(STATUS_C, m_registers.A >> 7);
        m_registers.A = (m_registers.A << 1) | carry;
        status_set_zn(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = (operand << 1) | carry;
        write(m_address, value);
        status_set_flag(STATUS_C, operand >> 7);
        status_set_zn(value);
    }

    return false;
}

bool CPU::op_ror()
{
    uint8_t carry = status_check_flag(STATUS_C);
    if (m_addressing_mode == AM_IMPLIED)
    {
        status_set_flag(STATUS_C, m_registers.A & 1);
        m_registers.A = (carry << 7) | (m_registers.A >> 1);
        status_set_zn(m_registers.A);
    }
    else
    {
        uint8_t operand = read(m_address);
        uint8_t value = (carry << 7) | (operand >> 1);
        write(m_address, value);
        status_set_flag(STATUS_C, operand & 1);
        status_set_zn(value);
    }

    return false;
}

bool CPU::op_bit()
{
    uint8_t operand = read(m_address);
    m_registers.P = (m_registers.P & ~0xC0) | (operand & 0xC0);
    status_set_flag(STATUS_Z, !(m_registers.A & operand));

    return false;
}

bool CPU::op_lax()
{
    m_registers.A = read(m_address);
    m_registers.X = m_registers.A;
    status_set_zn(m_registers.A);

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
    status_set_flag(STATUS_C, m_registers.X >= value);

    m_registers.X -= value;
    status_set_zn(m_registers.X);

    return false;
}

bool CPU::op_dcp()
{
    uint8_t operand = read(m_address) - 1;
    write(m_address, operand);
    uint8_t value = m_registers.A - operand;
    status_set_flag(STATUS_C, m_registers.A >= operand);
    status_set_zn(value);

    return false;
}

bool CPU::op_isc()
{
    uint8_t inc = read(m_address) + 1;
    write(m_address, inc);

    inc ^= 0xFF;
    uint8_t sign = (m_registers.A & 0x80) == (inc & 0x80);
    uint16_t value = m_registers.A + inc + status_check_flag(STATUS_C);
    m_registers.A = value & 0xFF;
    uint8_t overflow = sign && (m_registers.A & 0x80) != (inc & 0x80);

    status_set_flag(STATUS_C, (value & 0x100) >> 8);
    status_set_flag(STATUS_V, overflow);
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_slo()
{
    uint8_t operand = read(m_address);
    uint8_t value = operand << 1;
    status_set_flag(STATUS_C, operand >> 7);
    write(m_address, value);
    m_registers.A |= value;
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_rla()
{
    uint8_t operand = read(m_address);
    uint8_t value = (operand << 1) | (status_check_flag(STATUS_C) ? 1 : 0);
    status_set_flag(STATUS_C, operand >> 7);
    write(m_address, value);
    m_registers.A &= value;
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_sre()
{
    uint8_t operand = read(m_address);
    uint8_t value = operand >> 1;
    status_set_flag(STATUS_C, operand & 1);
    write(m_address, value);
    m_registers.A ^= value;
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_rra()
{
    uint8_t operand = read(m_address);
    uint8_t value = ((status_check_flag(STATUS_C) ? 1 : 0) << 7) | (operand >> 1);
    status_set_flag(STATUS_C, operand & 1);
    write(m_address, value);

    int sign = (m_registers.A >> 7) == (value >> 7);
    uint16_t result = m_registers.A + value + (status_check_flag(STATUS_C) ? 1 : 0);
    m_registers.A = result & 0xFF;
    uint8_t overflow = sign && (m_registers.A >> 7) != (value >> 7);

    status_set_flag(STATUS_C, (result & 0x100) >> 8);
    status_set_flag(STATUS_V, overflow);
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_anc()
{
    m_registers.A &= read(m_address);
    status_set_flag(STATUS_C, m_registers.A >> 7);
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_alr()
{
    m_registers.A &= read(m_address);
    status_set_flag(STATUS_C, m_registers.A & 1);
    m_registers.A >>= 1;
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_arr()
{
    m_registers.A &= read(m_address);
    m_registers.A = ((status_check_flag(STATUS_C) ? 1 : 0) << 7) | (m_registers.A >> 1);

    bool bit6 = (m_registers.A >> 6) & 1;
    bool bit5 = (m_registers.A >> 5) & 1;

    status_set_flag(STATUS_C, bit6);
    status_set_flag(STATUS_V, bit5 ^ bit6);
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_xaa()
{
    m_registers.A = m_registers.X;
    m_registers.A &= read(m_address);
    status_set_zn(m_registers.A);

    return false;
}

bool CPU::op_las()
{
    uint8_t value = m_registers.SP & read(m_address);
    m_registers.A = value;
    m_registers.X = value;
    m_registers.SP = value;
    status_set_zn(value);

    return true;
}

bool CPU::op_ahx()
{
    Word address;
    address.value = m_address + 1;
    write(m_address, m_registers.A & m_registers.X & address.byte_high);

    return false;
}

bool CPU::op_tas()
{
    Word address;
    address.value = m_address + 1;
    write(m_address, m_registers.A & m_registers.X & address.byte_high);
    m_registers.SP = m_registers.X & m_registers.A;

    return false;
}

bool CPU::op_shy()
{
    Word address;
    address.value = m_address;
    write(m_address, m_registers.Y & (address.byte_high + 1));

    return false;
}

bool CPU::op_shx()
{
    Word address;
    address.value = m_address;
    write(m_address, m_registers.X & (address.byte_high + 1));

    return false;
}

bool CPU::op_nop()
{
    return true;
}

bool CPU::op_hlt()
{
    LOG_WARNING("CPU: halt! opcode: %02X", m_opcode);
    return false;
}

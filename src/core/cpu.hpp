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

public:
    CPU(Memory* memory);
    ~CPU();

    void reset();

private:
    Registers m_registers;
    Memory* m_memory = nullptr;
    uint8_t m_cycles = 0;

    void set_flag(StatusFlags flag, bool value);
    bool check_flag(StatusFlags flag);
    void stack_push(uint8_t data);
    uint8_t stack_pop();
};

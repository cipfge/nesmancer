#pragma once

#include <cstdint>

class Controller
{
public:
    Controller();
    ~Controller();

    uint8_t read(uint8_t index);
    void write(uint8_t index, uint8_t data);

    uint8_t state(uint8_t index) const { return m_state[index]; }
    void set_state(uint8_t index, uint8_t state) { m_state[index] = state; }

private:
    uint8_t m_registers[2];
    uint8_t m_state[2];
};

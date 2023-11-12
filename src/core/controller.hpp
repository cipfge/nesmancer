#pragma once

#include <cstdint>

enum ButtonFlag
{
    BUTTON_FLAG_RIGHT = (1 << 0),
    BUTTON_FLAG_LEFT = (1 << 1),
    BUTTON_FLAG_DOWN = (1 << 2),
    BUTTON_FLAG_UP = (1 << 3),
    BUTTON_FLAG_START = (1 << 4),
    BUTTON_FLAG_SELECT = (1 << 5),
    BUTTON_FLAG_B = (1 << 6),
    BUTTON_FLAG_A = (1 << 7),
};

class Controller
{
public:
    Controller();
    ~Controller();

    uint8_t read(uint8_t index);
    void write(uint8_t index, uint8_t data);

    uint8_t state(uint8_t index) const { return m_registers[index]; }
    void set_state(uint8_t index, uint8_t state) { m_registers[index] = state; }

private:
    uint8_t m_registers[2];
};

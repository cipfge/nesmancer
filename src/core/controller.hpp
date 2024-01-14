#pragma once

#include "global.hpp"
#include <cstdint>

class InputManager;

class Controller
{
public:
    Controller(InputManager& input_manager):
        m_input_manager(input_manager)
    {}

    ~Controller() = default;

    uint8_t read(uint8_t index);
    void write(uint8_t data);

private:
    InputManager& m_input_manager;
    uint8_t m_registers[EMU_CONTROLLER_COUNT] = { 0, 0 };
    bool m_strobe = false;
};

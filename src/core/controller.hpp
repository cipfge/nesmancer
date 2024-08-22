#pragma once

#include <cstdint>

class InputManager;

class Controller
{
public:
    Controller(InputManager& input_manager):
        m_input_manager(input_manager)
    {}

    uint8_t read(uint8_t index);
    void write(uint8_t data);

    static constexpr uint8_t ControllerCount = 2;

private:
    InputManager& m_input_manager;
    uint8_t m_registers[ControllerCount] = { 0, 0 };
    bool m_strobe = false;
};

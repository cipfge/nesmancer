#include "controller.hpp"
#include "input_manager.hpp"
#include "logger.hpp"

Controller::Controller(InputManager& input_manager)
    : m_input_manager(input_manager)
{
}

uint8_t Controller::read(uint8_t index)
{
    if (index >= EMU_CONTROLLER_COUNT)
    {
        LOG_WARNING("Invalid controller index %u", index);
        return 0;
    }

    if (m_strobe)
        return 0x40 | (m_input_manager.get_buttons_state(index) & 0x1);

    uint8_t value = 0x40 | (m_registers[index] & 0x1);
    m_registers[index] = 0x80 | (m_registers[index] >> 1);

    return value;
}

void Controller::write(uint8_t data)
{
    if (m_strobe && !(data & 0x1))
    {
        for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
            m_registers[i] = m_input_manager.get_buttons_state(i);
    }

    m_strobe = (data & 0x1);
}

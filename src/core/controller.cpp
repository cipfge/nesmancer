#include "controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

uint8_t Controller::read(uint8_t index)
{
    uint8_t value = (m_registers[index] >> 7) & 1;
    m_registers[index] <<= 1;
    return value;
}

void Controller::write(uint8_t index, uint8_t data)
{
    if (data == 0)
        m_registers[index] = m_state[index];
}

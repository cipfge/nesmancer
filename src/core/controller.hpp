#pragma once

#include "types.hpp"

class Controller
{
public:
    Controller();
    ~Controller();

    uint8_t read(uint8_t index);
    void write(uint8_t index, uint8_t data);

private:
    uint8_t m_registers[2];
};

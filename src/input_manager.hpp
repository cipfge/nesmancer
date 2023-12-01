#pragma once

#include "global.hpp"
#include <cstdint>
#include <SDL.h>

class InputManager
{
public:
    InputManager() = default;
    ~InputManager() {}

    void process_input_event(SDL_Event& event);
    uint8_t get_buttons_state(uint8_t index);
    void search_controllers();
    uint8_t controller_count();

private:
    SDL_GameController* m_controllers[EMU_CONTROLLER_COUNT] = { nullptr, nullptr };

    void on_controller_connected(SDL_JoystickID id);
    void on_controller_disconnected(SDL_JoystickID id);
    void assign_controller(SDL_GameController* controller);
    void disconnect_controller(SDL_GameController* controller);
};

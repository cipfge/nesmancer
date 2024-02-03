#pragma once

#include "global.hpp"
#include <cstdint>
#include <SDL.h>

struct KeyboardConfig
{
    SDL_Scancode key_a[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_KP_2, SDL_SCANCODE_H };
    SDL_Scancode key_b[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_KP_3, SDL_SCANCODE_J };
    SDL_Scancode key_select[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_KP_5,SDL_SCANCODE_Y };
    SDL_Scancode key_start[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_KP_6, SDL_SCANCODE_U };
    SDL_Scancode key_up[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_UP, SDL_SCANCODE_W };
    SDL_Scancode key_down[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_DOWN, SDL_SCANCODE_S };
    SDL_Scancode key_left[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_LEFT, SDL_SCANCODE_A };
    SDL_Scancode key_right[EMU_CONTROLLER_COUNT] = { SDL_SCANCODE_RIGHT, SDL_SCANCODE_D };
};

class InputManager
{
public:
    InputManager() = default;
    ~InputManager();

    void process_input_event(SDL_Event& event);
    uint8_t get_buttons_state(uint8_t index);
    void search_controllers();
    uint8_t controller_count();

private:
    SDL_GameController* m_controllers[EMU_CONTROLLER_COUNT] = { nullptr, nullptr };
    KeyboardConfig m_key_config{};

    void on_controller_connected(SDL_JoystickID id);
    void on_controller_disconnected(SDL_JoystickID id);
    bool controller_assigned(SDL_JoystickID id);
    void assign_controller(SDL_GameController* controller);
};

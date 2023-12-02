#include "input_manager.hpp"
#include "controller.hpp"
#include "logger.hpp"

InputManager::~InputManager()
{
    for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
    {
        if (m_controllers[i])
            SDL_GameControllerClose(m_controllers[i]);
    }
}

void InputManager::process_input_event(SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_CONTROLLERDEVICEADDED:
        on_controller_connected(event.cdevice.which);
        break;

    case SDL_CONTROLLERDEVICEREMOVED:
        on_controller_disconnected(event.cdevice.which);
        break;

    default:
        break;
    }
}

uint8_t InputManager::get_buttons_state(uint8_t index)
{
    if (index >= EMU_CONTROLLER_COUNT)
    {
        LOG_WARNING("Invalid controller index %u", index);
        return 0;
    }

    uint8_t value = 0;

    SDL_GameController* controller = m_controllers[index];
    if (controller)
    {
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A)) << 0;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B)) << 1;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y)) << 2;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START)) << 3;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP)) << 4;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) << 5;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) << 6;
        value |= (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) << 7;

        static constexpr auto AxisThreshold = 8000;
        value |= (SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -AxisThreshold) << 4;
        value |= (SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > AxisThreshold) << 5;
        value |= (SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -AxisThreshold) << 6;
        value |= (SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > AxisThreshold) << 7;
    }
    else
    {
        uint8_t const* keyboard_state = SDL_GetKeyboardState(0);

        // TODO: Keyboard bindings for both controllers
        value |= (keyboard_state[SDL_SCANCODE_A]) << 0;
        value |= (keyboard_state[SDL_SCANCODE_S]) << 1;
        value |= (keyboard_state[SDL_SCANCODE_SPACE]) << 2;
        value |= (keyboard_state[SDL_SCANCODE_RETURN]) << 3;
        value |= (keyboard_state[SDL_SCANCODE_UP]) << 4;
        value |= (keyboard_state[SDL_SCANCODE_DOWN]) << 5;
        value |= (keyboard_state[SDL_SCANCODE_LEFT]) << 6;
        value |= (keyboard_state[SDL_SCANCODE_RIGHT]) << 7;
    }

    return value;
}

uint8_t InputManager::controller_count()
{
    uint8_t count = 0;
    for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
    {
        if (m_controllers[i])
            count++;
    }

    return count;
}

void InputManager::search_controllers()
{
    for (int id = 0; id < SDL_NumJoysticks(); id++)
    {
        if (!SDL_IsGameController(id) ||
            is_controller_assigned(id))
            continue;

        if (controller_count() >= EMU_CONTROLLER_COUNT)
            return;

        SDL_GameController* controller = SDL_GameControllerOpen(id);
        if (controller)
        {
            LOG_DEBUG("Controller %s connected", SDL_GameControllerName(controller));
            assign_controller(controller);
        }
    }
}

void InputManager::on_controller_connected(SDL_JoystickID id)
{
    if (is_controller_assigned(id) ||
        controller_count() >= EMU_CONTROLLER_COUNT)
        return;

    SDL_GameController* controller = SDL_GameControllerOpen(id);
    if (!controller)
    {
        LOG_WARNING("SDL_GameControllerOpen error: %s", SDL_GetError());
        return;
    }

    assign_controller(controller);
}

void InputManager::on_controller_disconnected(SDL_JoystickID id)
{
    for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
    {
        if (m_controllers[i] &&
            SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controllers[i])) == id)
        {
            LOG_DEBUG("Controller %s index %u disconnected", SDL_GameControllerName(m_controllers[i]), i);
            SDL_GameControllerClose(m_controllers[i]);
            m_controllers[i] = nullptr;
            search_controllers();

            return;
        }
    }
}

bool InputManager::is_controller_assigned(SDL_JoystickID id)
{
    for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
    {
        if (m_controllers[i] &&
            SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controllers[i])) == id)
        {
            return true;
        }
    }

    return false;
}

void InputManager::assign_controller(SDL_GameController* controller)
{
    for (int i = 0; i < EMU_CONTROLLER_COUNT; i++)
    {
        if (m_controllers[i])
            continue;

        m_controllers[i] = controller;
        LOG_DEBUG("Controller %s assigned to index %u", SDL_GameControllerName(m_controllers[i]), i);

        return;
    }
}

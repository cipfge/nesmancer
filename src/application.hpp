#pragma once

#include "global.hpp"
#include <string>
#include <SDL.h>

class Application
{
public:
    Application() = default;
    ~Application();

    int run(int argc, char* argv[]);
    void set_window_title(const std::string& title);

private:
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    SDL_GameController *m_controller = nullptr;

    std::string m_window_title = "NES";
    int m_window_width = EMU_DEFAULT_WINDOW_WIDTH;
    int m_window_height = EMU_DEFAULT_WINDOW_HEIGHT;
    bool m_running = false;

    bool init();
    void search_controller();
    void controller_connected(SDL_JoystickID id);
    void controller_disconnected(SDL_JoystickID id);
    void process_events();
    void process_keyboard_event(const SDL_Event& event);
    void process_controller_event(const SDL_Event& event);
    void process_window_event(const SDL_Event& event);
    void render();
};

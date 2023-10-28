#pragma once

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
    int m_window_width = (256 * 2);
    int m_window_height = (240 * 2);
    bool m_running = false;

    bool init();
    void search_controller();
    void process_events();
    void process_keyboard_event(const SDL_Event& event);
    void process_controller_event(const SDL_Event& event);
    void process_window_event(const SDL_Event& event);
    void render();
};

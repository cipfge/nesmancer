#pragma once

#include "global.hpp"
#include "device.hpp"
#include "version.hpp"
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
    Device m_nes;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_frame_texture = nullptr;
    SDL_GameController* m_controller = nullptr;

    std::string m_window_title = EMU_VERSION_NAME;
    int m_window_width = EMU_SCREEN_WIDTH * 2;
    int m_window_height = EMU_SCREEN_HEIGHT * 2;
    bool m_running = false;
    bool m_exit = false;
    bool m_paused = false;
    bool m_show_about = false;

    bool init();
    void search_controller();
    void controller_connected(SDL_JoystickID id);
    void controller_disconnected(SDL_JoystickID id);
    void process_events();
    void process_keyboard_event(const SDL_KeyboardEvent& event);
    void process_controller_event(const SDL_ControllerButtonEvent& event);
    void process_window_event(const SDL_Event& event);
    void render();
    void render_menubar();
    void render_exit_dialog();
    void render_about_dialog();

    void open_nes_file();
    void set_dark_theme();
};

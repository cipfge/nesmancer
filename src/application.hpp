#pragma once

#include "global.hpp"
#include "device.hpp"
#include "input_manager.hpp"
#include "version.hpp"
#include <string>
#include <SDL.h>

class Application
{
public:
    Application();
    ~Application();

    int run(int argc, char* argv[]);
    void set_window_title(const std::string& title);

private:
    Device m_nes;
    InputManager m_input_manager;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_frame_texture = nullptr;

    std::string m_window_title = EMU_VERSION_NAME;
    int m_window_width = EMU_SCREEN_WIDTH * 2;
    int m_window_height = EMU_SCREEN_HEIGHT * 2;
    bool m_running = false;
    bool m_exit = false;
    bool m_show_popup = false;
    bool m_show_about = false;

    bool init();
    void process_events();
    void on_keyboard_event(const SDL_KeyboardEvent& event);
    void on_window_event(const SDL_Event& event);
    void render();
    void render_menubar();
    void render_exit_dialog();
    void render_about_dialog();

    void open_nes_file();
    void set_dark_theme();
};

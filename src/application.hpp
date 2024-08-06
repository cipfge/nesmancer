#pragma once

#include "emulator.hpp"
#include "input_manager.hpp"
#include "cpu_widget.hpp"
#include "ppu_widget.hpp"
#include "version.hpp"
#include <string>
#include <memory>
#include <SDL.h>

class Application
{
public:
    Application() :
        m_nes(std::make_unique<Emulator>(m_input_manager)),
        m_cpu_widget(m_nes->cpu()),
        m_ppu_widget(m_nes->ppu())
    {}

    ~Application();

    int run(int argc, char* argv[]);
    void set_window_title(const std::string& title);
    SDL_Window* window() const { return m_window; }

private:
    InputManager m_input_manager;
    std::unique_ptr<Emulator> m_nes = nullptr;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_frame_texture = nullptr;
    SDL_version m_sdl_version = {};
    CpuWidget m_cpu_widget;
    PpuWidget m_ppu_widget;

    std::string m_window_title = EMU_VERSION_NAME;
    int m_window_width = PPU::ScreenWidth * 2;
    int m_window_height = PPU::ScreenHeight * 2;
    bool m_fullscreen = false;
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

    void toggle_fullscreen();
    void open_nes_file();
    void set_dark_theme();
};

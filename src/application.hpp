#pragma once

#include "emulator.hpp"
#include "input_manager.hpp"
#include "sound_queue.hpp"
#include "application_style.hpp"
#include "version.hpp"
#include <string>
#include <memory>
#include <SDL.h>

class Application
{
public:
    Application() :
        m_nes(std::make_unique<Emulator>(m_input_manager))
    {}

    ~Application();

    int run(int argc, char* argv[]);
    void set_window_title(const std::string& title);
    SDL_Window* window() const { return m_window; }

    static constexpr uint16_t DefaultWindowWidth = PPU::ScreenWidth * PPU::ScreenScale;
    static constexpr uint16_t DefaultWindowHeight = PPU::ScreenHeight * PPU::ScreenScale;

private:
    InputManager m_input_manager;
    std::unique_ptr<Emulator> m_nes = nullptr;
    std::unique_ptr<SoundQueue> m_sound_queue = nullptr;
    blip_sample_t m_sound_buffer[APU::SoundBufferSize] = {};
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_frame_texture = nullptr;
    SDL_version m_sdl_version = {};
    ApplicationStyle m_style;
    std::string m_style_name = "default";

    std::string m_window_title = EMU_VERSION_NAME;
    int m_window_width = DefaultWindowWidth;
    int m_window_height = DefaultWindowHeight;
    int m_window_x = SDL_WINDOWPOS_CENTERED;
    int m_window_y = SDL_WINDOWPOS_CENTERED;
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

    void load_settings();
    void save_settings();

    void toggle_fullscreen();
    void open_nes_file();
};

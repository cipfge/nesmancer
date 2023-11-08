#include "application.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>

Application::~Application()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GameControllerClose(m_controller);
    SDL_DestroyTexture(m_frame_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

int Application::run(int argc, char* argv[])
{
    m_running = init();
    if (!m_running)
        return -1;

    if (argc > 1 && !m_nes.load_rom(argv[1]))
        return -1;

    while (m_running)
    {
        process_events();
        m_nes.run_one_frame();
        render();
    }

    return 0;
}

void Application::set_window_title(const std::string& title)
{
    m_window_title = title;
    if (m_window)
        SDL_SetWindowTitle(m_window, m_window_title.c_str());
}

bool Application::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    m_window = SDL_CreateWindow(m_window_title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                m_window_width,
                                m_window_height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!m_window)
    {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window,
                                    -1, // Initialize the first rendering driver supporting the requested flags
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer)
    {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    m_frame_texture = SDL_CreateTexture(m_renderer,
                                 SDL_PIXELFORMAT_RGB888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 EMU_SCREEN_WIDTH,
                                 EMU_SCREEN_HEIGHT);
    if (!m_frame_texture)
    {
        std::cerr << "SDL_CreateTexture error: " << SDL_GetError() << "\n";
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    imgui_io.IniFilename = nullptr;

    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

    search_controller();

    return true;
}

void Application::search_controller()
{
    for (int id = 0; id < SDL_NumJoysticks(); id++)
    {
        if (SDL_IsGameController(id))
        {
            m_controller = SDL_GameControllerOpen(id);
            if (m_controller)
                return;
        }
    }
}

void Application::controller_connected(SDL_JoystickID id)
{
    if (!m_controller)
    {
        m_controller = SDL_GameControllerOpen(id);
        if (!m_controller)
            std::cerr << "SDL_GameControllerOpen error: " << SDL_GetError() << "\n";
    }
}

void Application::controller_disconnected(SDL_JoystickID id)
{
    if (m_controller &&
        SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controller)) == id)
    {
        SDL_GameControllerClose(m_controller);
        search_controller();
    }
}

void Application::process_events()
{
    SDL_Event event{};

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch(event.type)
        {
        case SDL_QUIT:
            // TODO: Confirm application exit
            m_running = false;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            process_keyboard_event(event);
            break;

        case SDL_CONTROLLERDEVICEADDED:
            controller_connected(event.cdevice.which);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            controller_disconnected(event.cdevice.which);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            process_controller_event(event);
            break;

        case SDL_WINDOWEVENT:
            process_window_event(event);
            break;

        default:
            break;
        }
    }
}

void Application::process_keyboard_event(const SDL_Event& event)
{
    // TODO: Keyboard events
    EMU_UNUSED(event);
}

void Application::process_controller_event(const SDL_Event& event)
{
    // TODO: Controller events
    EMU_UNUSED(event);
}

void Application::process_window_event(const SDL_Event& event)
{
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        m_window_width = event.window.data1;
        m_window_height = event.window.data2;
    }
}

void Application::render()
{
    SDL_RenderClear(m_renderer);

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    // TODO: User interface
    ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    SDL_UpdateTexture(m_frame_texture, nullptr, m_nes.frame_buffer(), EMU_SCREEN_WIDTH * sizeof(uint32_t));
    SDL_Rect window_rect = { 0, 0, m_window_width, m_window_height };
    SDL_RenderCopy(m_renderer, m_frame_texture, nullptr, &window_rect);
    SDL_RenderPresent(m_renderer);
}

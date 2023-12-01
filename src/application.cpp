#include "application.hpp"
#include "platform.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "logger.hpp"
#include <nfd.hpp>

#ifdef EMU_PLATFORM_WINDOWS
#include <SDL_syswm.h>
#endif // Windows

Application::Application()
{
    m_nes.set_input_manager(&m_input_manager);
}

Application::~Application()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

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

    if (!m_nes.init())
        return -1;

    if (argc > 1 && !m_nes.load_rom_file(argv[1]))
        return -1;

    // NTSC ~ 60 FPS?
    constexpr int DELAY = 1000.0f / 60;

    uint32_t frame_start = 0;
    uint32_t frame_time = 0;

    while (m_running)
    {
        frame_start = SDL_GetTicks();

        process_events();
        if (!m_show_popup)
            m_nes.run();
        render();

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < DELAY)
            SDL_Delay((int)(DELAY - frame_time));
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
        LOG_FATAL("SDL_Init error: %s", SDL_GetError());
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
        LOG_FATAL("SDL_CreateWindow error: %s", SDL_GetError());
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window,
                                    -1, // Initialize the first rendering driver supporting the requested flags
                                    SDL_RENDERER_ACCELERATED);
    if (!m_renderer)
    {
        LOG_FATAL("SDL_CreateRenderer error: %s", SDL_GetError());
        return false;
    }

    m_frame_texture = SDL_CreateTexture(m_renderer,
                                        SDL_PIXELFORMAT_RGB888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        EMU_SCREEN_WIDTH,
                                        EMU_SCREEN_HEIGHT);
    if (!m_frame_texture)
    {
        LOG_FATAL("SDL_CreateTexture error: %s", SDL_GetError());
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

    set_dark_theme();
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

    // Add menubar height
    m_window_height = m_window_height + (int)ImGui::GetFrameHeight();
    SDL_SetWindowSize(m_window, m_window_width, m_window_height);

    m_input_manager.search_controllers();

    return true;
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
            m_exit = true;
            break;

        case SDL_KEYDOWN:
            on_keyboard_event(event.key);
            break;

        case SDL_WINDOWEVENT:
            on_window_event(event);
            break;

        default:
            m_input_manager.process_input_event(event);
            break;
        }
    }
}

void Application::on_keyboard_event(const SDL_KeyboardEvent& event)
{
    if (event.repeat)
        return;

    if (event.keysym.sym == SDLK_o &&
        event.keysym.mod & KMOD_CTRL)
    {
        open_nes_file();
        return;
    }

    if (event.keysym.sym == SDLK_ESCAPE)
    {
        m_nes.toggle_pause();
        return;
    }

    if (event.keysym.sym == SDLK_r &&
        event.keysym.mod & KMOD_CTRL)
    {
        m_nes.reset();
        return;
    }
}

void Application::on_window_event(const SDL_Event& event)
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

    render_menubar();
    if (m_exit)
        ImGui::OpenPopup("Exit");
    render_exit_dialog();

    if (m_show_about)
        ImGui::OpenPopup("About");
    render_about_dialog();

    ImGui::EndFrame();

    if (m_nes.is_running())
    {
        SDL_UpdateTexture(m_frame_texture, nullptr, m_nes.screen(), EMU_SCREEN_WIDTH * sizeof(uint32_t));

        SDL_Rect window_rect = {
            0,
            (int)ImGui::GetFrameHeight(),
            m_window_width,
            m_window_height - (int)ImGui::GetFrameHeight()
        };

        SDL_RenderCopy(m_renderer, m_frame_texture, nullptr, &window_rect);
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(m_renderer);
}

void Application::render_menubar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open...", "Ctr+O"))
                open_nes_file();

            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4"))
                m_exit = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("System"))
        {
            const std::string menu_title = m_nes.is_paused() ? "Resume" : "Pause";
            if (ImGui::MenuItem(menu_title.c_str(), "Esc"))
                m_nes.toggle_pause();

            if (ImGui::MenuItem("Reset", "Ctr+R"))
                m_nes.reset();

            ImGui::Separator();
            if (ImGui::MenuItem("Power Off..."))
                m_nes.power_off();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About " EMU_VERSION_NAME))
                m_show_about = true;

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Application::render_exit_dialog()
{
    if (ImGui::BeginPopupModal("Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        m_show_popup = true;
        m_exit = false;

        ImGui::Text("Are you sure you want to exit?");
        ImGui::Separator();

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            m_running = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(120, 0)))
        {
            m_show_popup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Application::render_about_dialog()
{
    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        m_show_popup = true;
        m_show_about = false;

        ImGui::Text(EMU_VERSION_NAME);
        ImGui::Text("Version: %s", EMU_VERSION_NUMBER);
        ImGui::Separator();

        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            m_show_popup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Application::open_nes_file()
{
    NFD::Guard guard;
    NFD::UniquePath nes_file;

    nfdfilteritem_t filter[1] = {
        {"NES File", "nes"}
    };

#ifdef EMU_PLATFORM_WINDOWS
    // Set window owner
    SDL_SysWMinfo win_info {};
    SDL_VERSION(&win_info.version);
    SDL_GetWindowWMInfo(m_window, &win_info);
    NFD::SetWindowOwner(win_info.info.win.window);
#endif // Windows

    nfdresult_t result = NFD::OpenDialog(nes_file, filter, 1);
    if (result == NFD_OKAY)
        m_nes.load_rom_file(nes_file.get());
}

void Application::set_dark_theme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    style.WindowPadding     = ImVec2(8.00f, 8.00f);
    style.FramePadding      = ImVec2(5.00f, 2.00f);
    style.CellPadding       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing     = 25;
    style.ScrollbarSize     = 15;
    style.GrabMinSize       = 10;
    style.WindowBorderSize  = 1;
    style.ChildBorderSize   = 1;
    style.PopupBorderSize   = 1;
    style.FrameBorderSize   = 1;
    style.TabBorderSize     = 1;
    style.WindowRounding    = 7;
    style.ChildRounding     = 4;
    style.FrameRounding     = 3;
    style.PopupRounding     = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding      = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding       = 4;
}

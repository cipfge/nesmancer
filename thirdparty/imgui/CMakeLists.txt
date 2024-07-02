cmake_minimum_required(VERSION 3.22)

project(imgui VERSION 1.90.9 LANGUAGES CXX)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

set(IMGUI_SOURCES
    "src/imconfig.h"
    "src/imgui.cpp"
    "src/imgui_demo.cpp"
    "src/imgui_draw.cpp"
    "src/imgui.h"
    "src/imgui_impl_sdl2.cpp"
    "src/imgui_impl_sdl2.h"
    "src/imgui_impl_sdlrenderer2.cpp"
    "src/imgui_impl_sdlrenderer2.h"
    "src/imgui_internal.h"
    "src/imgui_tables.cpp"
    "src/imgui_widgets.cpp"
    "src/imstb_rectpack.h"
    "src/imstb_textedit.h"
    "src/imstb_truetype.h")

set(IMGUI_TARGET_NAME "imgui")

add_library(${IMGUI_TARGET_NAME} STATIC ${IMGUI_SOURCES})

target_include_directories(${IMGUI_TARGET_NAME}
    PRIVATE ${SDL2_INCLUDE_DIRS})

target_include_directories(${IMGUI_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${IMGUI_TARGET_NAME} INTERFACE SDL2::SDL2)

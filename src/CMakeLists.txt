find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

set(EMU_SOURCE_FILES
    "core/cartridge.cpp"
    "core/cartridge.hpp"
    "core/cpu.cpp"
    "core/cpu.hpp"
    "core/device.cpp"
    "core/device.hpp"
    "core/global.hpp"
    "core/mapper.cpp"
    "core/mapper.hpp"
    "core/memory.cpp"
    "core/memory.hpp"
    "core/ppu.cpp"
    "core/ppu.hpp"
    "core/types.hpp"
    "platform/platform.hpp"
    "application.cpp"
    "application.hpp"
    "main.cpp")

set(IMGUI_SOURCE_FILES
    "imgui/imconfig.h"
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui.h"
    "imgui/imgui_impl_sdl2.cpp"
    "imgui/imgui_impl_sdl2.h"
    "imgui/imgui_impl_sdlrenderer2.cpp"
    "imgui/imgui_impl_sdlrenderer2.h"
    "imgui/imgui_internal.h"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h")

if (WIN32)
    set(EMU_APPLICATION_TYPE "WIN32")
elseif(APPLE)
    set(EMU_APPLICATION_TYPE "MACOSX_BUNDLE")
else()
    set(EMU_APPLICATION_TYPE "")
endif()

add_executable(nes
    ${EMU_APPLICATION_TYPE}
    ${IMGUI_SOURCE_FILES}
    ${EMU_SOURCE_FILES})

target_link_libraries(nes PRIVATE SDL2::SDL2)
if (TARGET SDL2::SDL2main)
    target_link_libraries(nes PRIVATE SDL2::SDL2main)
endif()

target_include_directories(nes PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform")

set_target_properties(nes PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_compile_definitions(nes PRIVATE
    "$<$<CONFIG:Debug>:EMU_DEBUG_ENABLED>")

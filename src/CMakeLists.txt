find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

set(EMU_SOURCE_FILES
    "core/mappers/mapper.hpp"
    "core/mappers/mmc1.cpp"
    "core/mappers/mmc1.hpp"
    "core/mappers/nrom.cpp"
    "core/mappers/nrom.hpp"
    "core/mappers/uxrom.cpp"
    "core/mappers/uxrom.hpp"
    "core/mappers/cnrom.cpp"
    "core/mappers/cnrom.hpp"
    "core/apu.cpp"
    "core/apu.hpp"
    "core/cartridge.cpp"
    "core/cartridge.hpp"
    "core/controller.cpp"
    "core/controller.hpp"
    "core/cpu.cpp"
    "core/cpu.hpp"
    "core/emulator.cpp"
    "core/emulator.hpp"
    "core/global.hpp"
    "core/logger.hpp"
    "core/logger.cpp"
    "core/nes_file.cpp"
    "core/nes_file.hpp"
    "core/ppu.cpp"
    "core/ppu.hpp"
    "core/system_bus.cpp"
    "core/system_bus.hpp"
    "core/types.hpp"
    "platform/platform.hpp"
    "widgets/cpu_widget.cpp"
    "widgets/cpu_widget.hpp"
    "widgets/ppu_widget.cpp"
    "widgets/ppu_widget.hpp"
    "widgets/widget.hpp"
    "application.cpp"
    "application.hpp"
    "input_manager.cpp"
    "input_manager.hpp"
    "main.cpp"
    "utils.hpp")

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
    list(APPEND EMU_SOURCE_FILES "platform/windows/resources.rc")
elseif(APPLE)
    set(EMU_APPLICATION_TYPE "MACOSX_BUNDLE")
else()
    set(EMU_APPLICATION_TYPE "")
endif()

configure_file("version.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp")

add_executable(nesmancer
    ${EMU_APPLICATION_TYPE}
    ${IMGUI_SOURCE_FILES}
    ${EMU_SOURCE_FILES})

target_link_libraries(nesmancer PRIVATE SDL2::SDL2 nfd)
if (TARGET SDL2::SDL2main)
    target_link_libraries(nesmancer PRIVATE SDL2::SDL2main)
endif()

target_include_directories(nesmancer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/mappers"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets")

set_target_properties(nesmancer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_compile_definitions(nesmancer PRIVATE
    "$<$<CONFIG:Debug>:EMU_DEBUG_ENABLED>")

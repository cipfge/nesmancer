find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

set(EMU_SOURCE_FILES
    "core/mappers/mapper.cpp"
    "core/mappers/mapper.hpp"
    "core/mappers/mapper_cnrom.cpp"
    "core/mappers/mapper_cnrom.hpp"
    "core/mappers/mapper_mmc1.cpp"
    "core/mappers/mapper_mmc1.hpp"
    "core/mappers/mapper_mmc3.cpp"
    "core/mappers/mapper_mmc3.hpp"
    "core/mappers/mapper_nrom.cpp"
    "core/mappers/mapper_nrom.hpp"
    "core/mappers/mapper_uxrom.cpp"
    "core/mappers/mapper_uxrom.hpp"
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
    "core/ppu.cpp"
    "core/ppu.hpp"
    "core/system_bus.cpp"
    "core/system_bus.hpp"
    "core/types.hpp"
    "platform/platform.hpp"
    "application.cpp"
    "application.hpp"
    "application_style.cpp"
    "application_style.hpp"
    "common.hpp"
    "input_manager.cpp"
    "input_manager.hpp"
    "logger.hpp"
    "logger.cpp"
    "nes_rom.cpp"
    "nes_rom.hpp"
    "sound_queue.cpp"
    "sound_queue.hpp"
    "main.cpp")

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
    ${EMU_SOURCE_FILES})

if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT nesmancer)
endif()

target_link_libraries(nesmancer PRIVATE SDL2::SDL2 imgui nfd Nes_Snd_Emu)
if (TARGET SDL2::SDL2main)
    target_link_libraries(nesmancer PRIVATE SDL2::SDL2main)
endif()

target_include_directories(nesmancer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/mappers"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform"
    ${SDL2_INCLUDE_DIRS})

set_target_properties(nesmancer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_compile_definitions(nesmancer PRIVATE
    "$<$<CONFIG:Debug>:EMU_DEBUG_ENABLED>")

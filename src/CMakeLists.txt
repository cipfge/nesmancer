find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

set(EMU_SOURCE_FILES
    "core/global.hpp"
    "platform/platform.hpp"
    "application.cpp"
    "application.hpp"
    "main.cpp")

if (WIN32)
    set(EMU_APPLICATION_TYPE "WIN32")
elseif(APPLE)
    set(EMU_APPLICATION_TYPE "MACOSX_BUNDLE")
else()
    set(EMU_APPLICATION_TYPE "")
endif()

add_executable(nes ${EMU_APPLICATION_TYPE} ${EMU_SOURCE_FILES})

target_link_libraries(nes PRIVATE SDL2::SDL2)
if (TARGET SDL2::SDL2main)
    target_link_libraries(nes PRIVATE SDL2::SDL2main)
endif()

target_include_directories(nes PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform")

set_target_properties(nes PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_compile_definitions(nes PRIVATE
    "$<$<CONFIG:Debug>:EMU_DEBUG_ENABLED>")

cmake_minimum_required(VERSION 3.22)

project("Nesmancer" VERSION 0.1.1 LANGUAGES CXX)

set(EMU_DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(EMU_DEFAULT_BUILD_TYPE "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${EMU_DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${EMU_DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

if(UNIX AND NOT APPLE)
    set(NFD_PORTAL ON CACHE BOOL "Use xdg-desktop-portal instead of GTK" FORCE)
endif()

add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/nativefiledialog)
add_subdirectory(thirdparty/Nes_Snd_Emu)
add_subdirectory(src)

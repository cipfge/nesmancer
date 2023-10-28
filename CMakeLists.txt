cmake_minimum_required(VERSION 3.25)

project(nes-emulator VERSION 0.1.0 LANGUAGES CXX)

set(EMU_DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(EMU_DEFAULT_BUILD_TYPE "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${EMU_DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${EMU_DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

add_subdirectory(src)

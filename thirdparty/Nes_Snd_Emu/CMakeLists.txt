CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Nes_Snd_Emu)

SET(PACKAGE "Nes_Snd_Emu")
SET(PACKAGE_NAME ${PACKAGE})
SET(PACKAGE_VERSION_MAJOR "0")
SET(PACKAGE_VERSION_MINOR "2")
SET(PACKAGE_VERSION_PATCH "0")
SET(VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
SET(PACKAGE_VERSION ${VERSION})
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

# Targets
SET(NES_SND_EMU_SOURCES
    emu2413/emu2413.c
    nes_apu/Blip_Buffer.cpp
    nes_apu/Multi_Buffer.cpp
    nes_apu/Nes_Apu.cpp
    nes_apu/Nes_Fds_Apu.cpp
    nes_apu/Nes_Fme7_Apu.cpp
    nes_apu/Nes_Mmc5_Apu.cpp
    nes_apu/Nes_Namco_Apu.cpp
    nes_apu/Nes_Oscs.cpp
    nes_apu/Nes_Vrc6_Apu.cpp
    nes_apu/Nes_Vrc7_Apu.cpp
)

SET(NES_SND_EMU_HEADERS
    emu2413/emu2413.h
    nes_apu/Blip_Buffer.h
    nes_apu/Blip_Buffer_impl.h
    nes_apu/Blip_Buffer_impl2.h
    nes_apu/dllexport.h
    nes_apu/Multi_Buffer.h
    nes_apu/Nes_Apu.h
    nes_apu/Nes_Fds_Apu.h
    nes_apu/Nes_Fme7_Apu.h
    nes_apu/Nes_Mmc5_Apu.h
    nes_apu/Nes_Namco_Apu.h
    nes_apu/Nes_Oscs.h
    nes_apu/Nes_Vrc6_Apu.h
    nes_apu/Nes_Vrc7_Apu.h
)

ADD_LIBRARY(Nes_Snd_Emu SHARED
    ${NES_SND_EMU_SOURCES}
    ${NES_SND_EMU_HEADERS}
)

SET_TARGET_PROPERTIES(Nes_Snd_Emu 
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"    
        VERSION ${VERSION}
)

TARGET_INCLUDE_DIRECTORIES(Nes_Snd_Emu PUBLIC ${PROJECT_SOURCE_DIR} emu2413)
TARGET_COMPILE_DEFINITIONS(Nes_Snd_Emu PUBLIC NES_SND_DYNAMIC PRIVATE NES_SND_BUILD)
TARGET_COMPILE_FEATURES(Nes_Snd_Emu PUBLIC cxx_std_11)

IF(MSVC)
	# windows.h defines min and max, and std::min and std::max aren't defined in <algorithm> without this macro
	TARGET_COMPILE_DEFINITIONS(Nes_Snd_Emu PRIVATE NOMINMAX)
	# stdlib.h on Windows defines min and max preprocessor macros by default as a "language extensions"
	TARGET_COMPILE_DEFINITIONS(Nes_Snd_Emu PRIVATE _CRT_DECLARE_NONSTDC_NAMES=0)
ENDIF()

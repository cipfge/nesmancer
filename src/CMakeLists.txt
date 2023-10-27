set(EMU_SOURCE_FILES "main.cpp")

add_executable(nes ${EMU_SOURCE_FILES})

target_include_directories(nes PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(nes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

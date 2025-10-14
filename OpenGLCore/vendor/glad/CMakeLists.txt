cmake_minimum_required(VERSION 3.15)
project(Glad C)

# Output directories for binary and intermediate files
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Glad)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Glad)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/$<CONFIG>/Glad)

# Create the static library
add_library(Glad STATIC
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

# Set include directories
target_include_directories(Glad PUBLIC include)

# OS-specific settings
if(WIN32)
    # Windows specific settings
    # (systemversion "latest" is generally the default on modern CMake/Visual Studio)
endif()

# Configuration-specific compile options
target_compile_options(Glad PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)

target_compile_definitions(Glad PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

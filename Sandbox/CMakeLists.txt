# Define the project
project(Sandbox)

# Set C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static runtime (for MSVC)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

# Set output directories
set(TARGET_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})

# Collect source files
file(GLOB_RECURSE SANDBOX_SOURCES
        "src/*.cpp"
        "src/*.hpp"
)

# Define executable
add_executable(Sandbox ${SANDBOX_SOURCES})

# Include directories
target_include_directories(Sandbox PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ../VoxelEngine/src
)

# Link with other libraries
target_link_libraries(Sandbox PRIVATE
        VoxelEngine
)
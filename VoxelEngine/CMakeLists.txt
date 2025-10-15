# Define the project
project(VoxelEngine)

# Set C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static runtime (for MSVC)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set output directories
set(TARGET_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/lib")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})

# Collect source files
file(GLOB_RECURSE VOXEL_ENGINE_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "vendor/PerlinNoise/*.hpp"
    "vendor/Entt/*.hpp"
)

# Define executable
add_library(VoxelEngine STATIC ${VOXEL_ENGINE_SOURCES})

add_custom_command(TARGET VoxelEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:VoxelEngine>/../assets
)

# Include directories
target_include_directories(VoxelEngine PUBLIC
    ../OpenGLCore/vendor/spdlog/include
    ../OpenGLCore/src
    ../OpenGLCore/vendor
    ../OpenGLCore/vendor/glm
    ../OpenGLCore/vendor/Glad/include
    ../OpenGLCore/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/JoltPhysics
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Assimp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/PerlinNoise
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Entt
)

# Link with other libraries
target_link_libraries(VoxelEngine PUBLIC
    OpenGLCore
    JoltPhysics
    Assimp
)
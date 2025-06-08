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
set(TARGET_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(OBJECT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OBJECT_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OBJECT_OUTPUT_DIR})

# Collect source files
file(GLOB_RECURSE VOXELENGINE_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "vendor/PerlinNoise/*.hpp"
    "vendor/Entt/*.hpp"
)

# Define executable
add_executable(VoxelEngine ${VOXELENGINE_SOURCES})

add_custom_command(TARGET VoxelEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:VoxelEngine>/assets
)

# Include directories
target_include_directories(VoxelEngine PRIVATE
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
target_link_libraries(VoxelEngine PRIVATE
    OpenGLCore
    JoltPhysics
    Assimp
)

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(VoxelEngine PRIVATE GLCORE_PLATFORM_WINDOWS)
endif()

# Configuration-specific settings
target_compile_definitions(VoxelEngine PRIVATE
    $<$<CONFIG:Debug>:GLCORE_DEBUG>
    $<$<CONFIG:Release>:GLCORE_RELEASE>
)

# Optional: set debug symbols and optimization manually if needed
# These are usually set by default by CMake presets

cmake_minimum_required(VERSION 3.15)
project(VoxelEngine)

# Set the architecture
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Static runtime (for MSVC)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Define configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Output directory setup
set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

# Include directories
set(IncludeDir
    ${CMAKE_SOURCE_DIR}/vendor/GLFW/include
    ${CMAKE_SOURCE_DIR}/vendor/Glad/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/stb_image
    ${CMAKE_SOURCE_DIR}/vendor/JoltPhysics
    ${CMAKE_SOURCE_DIR}/vendor/Assimp/include
    ${CMAKE_SOURCE_DIR}/vendor/PerlinNoise
    ${CMAKE_SOURCE_DIR}/vendor/Entt
)

# Add dependencies as subdirectories
add_subdirectory(OpenGLCore/vendor/GLFW)
add_subdirectory(OpenGLCore/vendor/Glad)
add_subdirectory(OpenGLCore/vendor/imgui)
add_subdirectory(VoxelEngine/vendor/JoltPhysics)
add_subdirectory(VoxelEngine/vendor/Assimp)

# Add main projects
add_subdirectory(OpenGLCore)
add_subdirectory(VoxelEngine)

cmake_minimum_required(VERSION 3.15)
project(VoxelEngineRoot)

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

# Add dependencies as subdirectories
add_subdirectory(OpenGLCore/vendor/GLFW)
add_subdirectory(OpenGLCore/vendor/Glad)
add_subdirectory(OpenGLCore/vendor/imgui)
add_subdirectory(VoxelEngine/vendor/JoltPhysics)
add_subdirectory(VoxelEngine/vendor/Assimp)

# Add main projects
add_subdirectory(OpenGLCore)
add_subdirectory(VoxelEngine)
add_subdirectory(Sandbox)

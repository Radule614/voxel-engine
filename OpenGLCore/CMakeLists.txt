project(OpenGLCore)

# C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static runtime (MSVC only)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Output directories
set(TARGET_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(OBJECT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})

# Collect source files
file(GLOB_RECURSE OPENGLCORE_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "vendor/stb_image/*.h"
    "vendor/stb_image/*.cpp"
    "vendor/glm/glm/**/*.hpp"
    "vendor/glm/glm/**/*.cpp"
)

# Create static library
add_library(OpenGLCore STATIC ${OPENGLCORE_SOURCES})

# Precompiled header (glpch.hpp/glpch.cpp)
target_precompile_headers(OpenGLCore PRIVATE src/glpch.hpp)

# Preprocessor definitions
target_compile_definitions(OpenGLCore PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

# Include directories
target_include_directories(OpenGLCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image
)

# Linked libraries
target_link_libraries(OpenGLCore PRIVATE
    GLFW
    Glad
    ImGui
    opengl32
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(OpenGLCore PRIVATE
        GLCORE_PLATFORM_WINDOWS
        GLFW_INCLUDE_NONE
    )
endif()

# Config-specific defines
target_compile_definitions(OpenGLCore PRIVATE
    $<$<CONFIG:Debug>:GLCORE_DEBUG>
    $<$<CONFIG:Release>:GLCORE_RELEASE>
)

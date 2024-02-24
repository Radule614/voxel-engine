cmake_minimum_required(VERSION 3.0.0)
project(OpenGLCore VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

add_library(OpenGLCore STATIC ${SOURCE_FILES})
target_link_libraries(OpenGLCore glfw)
target_link_libraries(OpenGLCore glad)

target_include_directories(OpenGLCore PUBLIC include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

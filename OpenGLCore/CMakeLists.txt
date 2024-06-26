cmake_minimum_required(VERSION 3.20)
project(OpenGLCore VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/stb_image)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(OpenGLCore STATIC ${SOURCE_FILES})
target_link_libraries(OpenGLCore glfw)
target_link_libraries(OpenGLCore glad)
target_link_libraries(OpenGLCore spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_64>)
target_link_libraries(OpenGLCore glm)
target_link_libraries(OpenGLCore stb_image)

target_include_directories(OpenGLCore PUBLIC src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

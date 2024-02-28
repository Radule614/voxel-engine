cmake_minimum_required(VERSION 3.20)
project(VoxelEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(OpenGLCore)

file(GLOB_RECURSE SOURCE_FILES VoxelEngine/*.cpp)

add_executable(VoxelEngine ${SOURCE_FILES})
target_link_libraries(VoxelEngine PRIVATE OpenGLCore)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

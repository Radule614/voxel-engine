cmake_minimum_required(VERSION 3.0.0)
project(VoxelEngine VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

add_executable(VoxelEngine ${SOURCE_FILES})
target_link_libraries(VoxelEngine glfw)
target_link_libraries(VoxelEngine glad)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cmake_minimum_required(VERSION 3.10)
project(VoxelEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(OpenGLCore)
# link_directories(VoxelEngine/vendor/BulletPhysics)
file(GLOB_RECURSE SOURCE_FILES VoxelEngine/*.cpp)

add_executable(VoxelEngine ${SOURCE_FILES})
target_link_libraries(VoxelEngine PUBLIC OpenGLCore)
# target_link_libraries(VoxelEngine PUBLIC BulletCollision.lib BulletDynamics.lib BulletSoftBody.lib LinearMath.lib)
target_include_directories(VoxelEngine PUBLIC VoxelEngine/vendor/PerlinNoise)
# target_include_directories(VoxelEngine PUBLIC VoxelEngine/vendor/BulletPhysics/include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_BUILD_TYPE "Release")

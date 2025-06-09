#pragma once

#include <GLCoreUtils.hpp>
#include <glm/glm.hpp>

namespace VoxelEngine
{

inline int Cantor(int32_t a, int32_t b)
{
    return (a + b + 1) * (a + b) / 2 + b;
}

inline bool InRange(int32_t v, int32_t l, int32_t r)
{
    return l <= v && v <= r;
}

inline void PrintVec2(const glm::vec2& vec)
{
    LOG_INFO("({0}, {1})", vec.x, vec.y);
}

inline void PrintVec3(const glm::vec3& vec)
{
    LOG_INFO("({0}, {1}, {2})", vec.x, vec.y, vec.z);
}

}

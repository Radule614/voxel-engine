#pragma once

#include <GLCoreUtils.hpp>
#include <glm/glm.hpp>
#include <format>
#include "../Terrain/Utils/Position2D.hpp"

namespace VoxelEngine
{

inline int Cantor(int32_t a, int32_t b) { return (a + b + 1) * (a + b) / 2 + b; }

inline bool InRange(int32_t v, int32_t l, int32_t r) { return l <= v && v <= r; }

inline std::string VecToString(const glm::vec2& vec) { return std::format("Vec2({}, {})", vec.x, vec.y); }

inline std::string VecToString(const glm::vec3& vec) { return std::format("Vec3({}, {}, {})", vec.x, vec.y, vec.z); }

inline void PrintVec2(const glm::vec2& vec) { LOG_INFO("{0}", VecToString(vec)); }

inline void PrintVec3(const glm::vec3& vec) { LOG_INFO("{0}", VecToString(vec)); }

inline float_t GetDistance(const Position2D a, const Position2D b)
{
    return glm::distance((glm::vec2) a, (glm::vec2) b);
}

}

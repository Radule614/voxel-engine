#pragma once

#include <GLCoreUtils.hpp>
#include <glm/glm.hpp>
#include <format>

namespace VoxelEngine
{

inline int Cantor(int32_t a, int32_t b) { return (a + b + 1) * (a + b) / 2 + b; }

inline bool InRange(int32_t v, int32_t l, int32_t r) { return l <= v && v <= r; }

inline std::string VecToString(const glm::vec2& vec) { return std::format("Vec2({}, {})", vec.x, vec.y); }

inline std::string VecToString(const glm::vec3& vec) { return std::format("Vec3({}, {}, {})", vec.x, vec.y, vec.z); }

inline void PrintVec2(const glm::vec2& vec) { LOG_INFO(VecToString(vec)); }

inline void PrintVec3(const glm::vec3& vec) { LOG_INFO(VecToString(vec)); }

}

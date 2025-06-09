#pragma once

#include <glm/glm.hpp>
#include "../Utils/Utils.hpp"
#include <string>

namespace VoxelEngine
{

struct Position3D
{
    uint8_t xz; // 4 bits for x and 4 bits for z
    uint8_t y;

    Position3D() : xz(0), y(0)
    {
    }

    Position3D(uint8_t x, uint8_t y, uint8_t z) : xz((x & 0xF) | ((z & 0xF) << 4)), y(y)
    {
    }

    Position3D(glm::i16vec3 v) : Position3D(v.x, v.y, v.z)
    {
    }

    inline uint8_t GetX() const
    {
        return xz & 0xF;
    }

    inline uint8_t GetZ() const
    {
        return (xz >> 4) & 0xF;
    }

    inline void SetX(uint8_t x)
    {
        xz = (xz & 0xF0) | (x & 0xF);
    }

    inline void SetZ(uint8_t z)
    {
        xz = (xz & 0x0F) | ((z & 0xF) << 4);
    }

    bool operator<(const Position3D& pos) const noexcept
    {
        if (y != pos.y)
            return y < pos.y;
        if (GetZ() != pos.GetZ())
            return GetZ() < pos.GetZ();
        return GetX() < pos.GetX();
    }

    bool operator==(const Position3D& pos) const noexcept
    {
        return GetX() == pos.GetX() && y == pos.y && GetZ() == pos.GetZ();
    }

    Position3D operator+(Position3D const& other)
    {
        Position3D res;
        res.SetX(GetX() + other.GetX());
        res.y = y + other.y;
        res.SetZ(GetZ() + other.GetZ());
        return res;
    }

    inline double_t GetLength() const
    {
        return glm::sqrt(glm::pow(GetX(), 2) + glm::pow(y, 2) + glm::pow(GetZ(), 2));
    }

    operator glm::i16vec3() const
    {
        return glm::i16vec3(GetX(), y, GetZ());
    }

    operator glm::vec3() const
    {
        return glm::vec3(GetX(), y, GetZ());
    }

    inline std::string ToString() const
    {
        return "(" + std::to_string(GetX()) + ", " + std::to_string(y) + ", " + std::to_string(GetZ()) + ")";
    }
};

};

namespace std
{
using namespace VoxelEngine;

template<>
struct std::hash<Position3D>
{
    std::size_t operator()(const Position3D& p) const
    {
        return Cantor(p.GetX(), Cantor(p.y, p.GetZ()));
    }
};
};

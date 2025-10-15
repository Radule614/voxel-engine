#pragma once

#include <glm/glm.hpp>
#include "../../Utils/Utils.hpp"

namespace VoxelEngine
{

struct Position2D
{
    int16_t x;
    int16_t y;

    Position2D() : x(0), y(0)
    {
    }

    Position2D(int16_t x, int16_t y) : x(x), y(y)
    {
    }

    bool operator<(const Position2D& pos) const noexcept
    {
        if (y != pos.y)
            return y < pos.y;
        return x < pos.x;
    }

    bool operator==(const Position2D& pos) const noexcept { return x == pos.x && y == pos.y; }

    Position2D operator+(Position2D const& other) const
    {
        Position2D res;
        res.x = x + other.x;
        res.y = y + other.y;
        return res;
    }

    double_t GetLength() const { return glm::sqrt(glm::pow(x, 2) + glm::pow(y, 2)); }

    explicit operator glm::i32vec2() const { return {x, y}; }

    explicit operator glm::vec2() const { return {x, y}; }

    std::string ToString() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
};

};


template<>
struct std::hash<VoxelEngine::Position2D>
{
    std::size_t operator()(const VoxelEngine::Position2D& p) const noexcept { return VoxelEngine::Cantor(p.x, p.y); }
};

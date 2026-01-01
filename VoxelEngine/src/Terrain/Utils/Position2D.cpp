//
// Created by RadU on 10/18/2025.
//

#include "Position2D.hpp"

#include "../../Utils/Utils.hpp"

namespace VoxelEngine
{

Position2D::Position2D() : x(0), y(0)
{
}

Position2D::Position2D(int16_t x, int16_t y) : x(x), y(y)
{
}

bool Position2D::operator<(const Position2D& pos) const noexcept
{
    if (y != pos.y)
        return y < pos.y;

    return x < pos.x;
}


Position2D Position2D::operator+(Position2D const& other) const
{
    Position2D res;
    res.x = x + other.x;
    res.y = y + other.y;
    return res;
}

Position2D::operator glm::i32vec2() const { return {x, y}; }

Position2D::operator glm::vec2() const { return {x, y}; }

std::string Position2D::ToString() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

}

std::size_t std::hash<VoxelEngine::Position2D>::operator()(const VoxelEngine::Position2D& p) const noexcept
{
    return VoxelEngine::Cantor(p.x, p.y);
}

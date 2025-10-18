//
// Created by RadU on 10/18/2025.
//

#include "Position3D.hpp"

namespace VoxelEngine
{

Position3D::Position3D() : xz(0), y(0)
{
}

Position3D::Position3D(const uint8_t x, const uint8_t y, const uint8_t z) : xz((x & 0xF) | ((z & 0xF) << 4)), y(y)
{
}

Position3D::Position3D(const glm::i32vec3 v) : Position3D(v.x, v.y, v.z)
{
}

uint8_t Position3D::GetX() const { return xz & 0xF; }

uint8_t Position3D::GetY() const { return y; }

uint8_t Position3D::GetZ() const { return (xz >> 4) & 0xF; }

void Position3D::SetX(const uint8_t x) { xz = (xz & 0xF0) | (x & 0xF); }

void Position3D::SetY(const uint8_t y) { this->y = y; }

void Position3D::SetZ(const uint8_t z) { xz = (xz & 0x0F) | ((z & 0xF) << 4); }

bool Position3D::operator<(const Position3D& pos) const noexcept
{
    if (y != pos.y)
        return y < pos.y;
    if (GetZ() != pos.GetZ())
        return GetZ() < pos.GetZ();
    return GetX() < pos.GetX();
}

bool Position3D::operator==(const Position3D& pos) const noexcept
{
    return GetX() == pos.GetX() && y == pos.y && GetZ() == pos.GetZ();
}

Position3D Position3D::operator+(Position3D const& other) const
{
    Position3D res;
    res.SetX(GetX() + other.GetX());
    res.y = y + other.y;
    res.SetZ(GetZ() + other.GetZ());
    return res;
}

Position3D::operator glm::ivec3() const { return {GetX(), y, GetZ()}; }

Position3D::operator glm::vec3() const { return {GetX(), y, GetZ()}; }

std::string Position3D::ToString() const
{
    return "(" + std::to_string(GetX()) + ", " + std::to_string(y) + ", " + std::to_string(GetZ()) + ")";
}

}

std::size_t std::hash<VoxelEngine::Position3D>::operator()(const VoxelEngine::Position3D& p) const noexcept
{
    return VoxelEngine::Cantor(p.GetX(), VoxelEngine::Cantor(p.GetY(), p.GetZ()));
}

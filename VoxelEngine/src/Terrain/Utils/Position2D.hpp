#pragma once

#include <glm/glm.hpp>
#include <string>

namespace VoxelEngine
{

struct Position2D
{
    int16_t x;
    int16_t y;

    Position2D();
    Position2D(int16_t x, int16_t y);

    bool operator<(const Position2D& pos) const noexcept;

    bool operator==(const Position2D& pos) const noexcept { return x == pos.x && y == pos.y; }

    Position2D operator+(Position2D const& other) const;
    explicit operator glm::i32vec2() const;
    explicit operator glm::vec2() const;

    std::string ToString() const;
};

};

template<>
struct std::hash<VoxelEngine::Position2D>
{
    std::size_t operator()(const VoxelEngine::Position2D& p) const noexcept;
};

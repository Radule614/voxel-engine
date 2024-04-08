#pragma once

namespace Terrain
{
struct Position2D
{
    int16_t x;
    int16_t y;

    bool operator<(const Position2D &pos) const noexcept
    {
        if (y != pos.y)
            return y < pos.y;
        return x < pos.x;
    }

    bool operator==(const Position2D &pos) const noexcept
    {
        return x == x && y == y;
    }

    Position2D() : Position2D(0, 0)
    {
    }

    Position2D(int16_t x, int16_t y) : x(x), y(y)
    {
    }
};
}; // namespace Terrain

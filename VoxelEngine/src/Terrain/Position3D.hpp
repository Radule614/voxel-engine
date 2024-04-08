#pragma once

namespace Terrain
{
struct Position3D
{
    int16_t x;
    int16_t y;
    int16_t z;

    bool operator<(const Position3D &pos) const noexcept
    {
        if (y != pos.y)
            return y < pos.y;
        if (x != pos.x)
            return x < pos.x;
        return z < pos.z;
    }

    bool operator==(const Position3D &pos) const noexcept
    {
        return x == x && y == y;
    }

    Position3D() : Position3D(0, 0, 0)
    {
    }

    Position3D(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z)
    {
    }
};
}; // namespace Terrain

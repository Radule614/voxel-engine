#pragma once

#include <glm/glm.hpp>

namespace Terrain
{
struct MapPosition
{
    glm::vec2 Vector;

    bool operator<(const MapPosition &pos) const noexcept
    {
        if (this->Vector.y != pos.Vector.y)
            return this->Vector.y < pos.Vector.y;
        return this->Vector.x < pos.Vector.x;
    }

    bool operator==(const MapPosition &pos) const noexcept
    {
        return this->Vector == pos.Vector;
    }

    MapPosition(glm::vec2 pos) : Vector(pos)
    {
    }
};
}; // namespace Terrain

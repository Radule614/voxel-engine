#pragma once

#include <glm/glm.hpp>

namespace Terrain
{
struct MapPosition
{
    glm::vec3 Vector;

    bool operator<(const MapPosition &pos) const noexcept
    {
        if (this->Vector.y != pos.Vector.y)
            return this->Vector.y < pos.Vector.y;
        if (this->Vector.z != pos.Vector.z)
            return this->Vector.z < pos.Vector.z;
        return this->Vector.x < pos.Vector.x;
    }

    MapPosition(glm::vec3 pos) : Vector(pos)
    {
    }
};
}; // namespace Terrain

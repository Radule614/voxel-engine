#pragma once

#include <glm/glm.hpp>

namespace Terrain
{
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Texture;
};
}; // namespace Terrain

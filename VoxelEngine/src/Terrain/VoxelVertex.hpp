//
// Created by RadU on 7/27/2025.
//

#pragma once

#include <glm/glm.hpp>

namespace VoxelEngine
{

struct VoxelVertex
{
    glm::vec3 Position;
    uint32_t RadianceBaseIndex;
    uint8_t Face;
    glm::vec2 TexCoords;
};

}

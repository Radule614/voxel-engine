//
// Created by RadU on 7/27/2025.
//

#pragma once

#include <glm/glm.hpp>
#include "VoxelConstants.hpp"

namespace VoxelEngine
{

struct VoxelVertex
{
    glm::vec3 Position;
    uint8_t Face;
    glm::vec2 TexCoords;

    void SetFace(const VoxelFace face)
    {
        Face = 0;
        Face |= 1 << static_cast<uint8_t>(face);
    }
};

}

//
// Created by RadU on 12/14/2025.
//

#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace VoxelEngine
{

struct Material
{
    glm::vec4 BaseColorFactor;
    GLenum TextureId;
};

}
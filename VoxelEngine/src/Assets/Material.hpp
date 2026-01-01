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
    glm::vec4 AlbedoFactor;
    GLenum AlbedoTextureId = 0;

    float_t MetallicFactor;
    float_t RoughnessFactor;
    GLenum MetallicRoughnessTextureId = 0;

    GLenum AmbientOcclusionTextureId = 0;
    float_t AmbientOcclusionStrength;

    GLenum NormalTextureId = 0;
    float_t NormalScale;
};

}

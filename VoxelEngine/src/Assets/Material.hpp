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
    GLenum AlbedoTextureId;

    float_t MetallicFactor;
    float_t RoughnessFactor;
    GLenum MetallicRoughnessTextureId;

    GLenum AmbientOcclusionTextureId;
    float_t AmbientOcclusionStrength;

    GLenum NormalTextureId;
    float_t NormalScale;
};

}

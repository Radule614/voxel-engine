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
    glm::vec4 AlbedoFactor = glm::vec4(1.0f);
    GLenum AlbedoTextureId = 0;

    float_t MetallicFactor = 1.0f;
    float_t RoughnessFactor = 1.0f;
    GLenum MetallicRoughnessTextureId = 0;

    GLenum AmbientOcclusionTextureId = 0;
    float_t AmbientOcclusionStrength = 1.0f;

    GLenum NormalTextureId = 0;
    float_t NormalScale = 1.0f;
};

}

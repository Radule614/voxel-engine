//
// Created by RadU on 12/13/2025.
//

#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace VoxelEngine
{

struct Material
{
    glm::vec4 BaseColorFactor;
    int32_t TextureId;
};

struct RenderPrimitive
{
    GLuint Vao;
    GLenum Mode;
    GLsizei IndexCount;
    GLenum IndexType;

    Material Material{};
};

}

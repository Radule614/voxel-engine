//
// Created by RadU on 12/13/2025.
//

#pragma once

#include "Material.hpp"

namespace VoxelEngine
{

struct RenderPrimitive
{
    GLuint Vao;
    GLenum Mode;
    GLsizei IndexCount;
    GLenum IndexType;

    Material Material{};
};

}

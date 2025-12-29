//
// Created by RadU on 12/29/2025.
//

#pragma once

#include "glad/glad.h"
#include "glm/vec3.hpp"

namespace VoxelEngine
{

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 LightColor;
    GLuint DepthCubeMap = 0;

    PointLight(const glm::vec3 position, const glm::vec3 color) : Position(position), LightColor(color)
    {
    }
};

}

//
// Created by RadU on 12/29/2025.
//

#pragma once

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

namespace VoxelEngine
{

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 LightColor;
    GLuint DepthCubeMap;

    PointLight(glm::vec3 position, glm::vec3 color);

    std::vector<glm::mat4> CalculateShadowTransforms() const;
};

}

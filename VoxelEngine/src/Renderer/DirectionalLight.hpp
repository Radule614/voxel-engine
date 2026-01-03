//
// Created by RadU on 1/2/2026.
//

#pragma once

#include "glm/glm.hpp"

namespace VoxelEngine
{

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec3 LightColor;

    explicit DirectionalLight(glm::vec3 direction, glm::vec3 color = glm::vec3(1.0f));

    glm::mat4 GetLightSpaceTransform(glm::vec3 cameraPosition) const;
};

}

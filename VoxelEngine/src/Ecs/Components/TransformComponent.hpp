#pragma once

#include <GLCoreUtils.hpp>
#include <glm/glm.hpp>

namespace VoxelEngine
{

struct TransformComponent
{
    glm::vec3 Position;
    float_t RotationAngle;
    glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Scale;
    glm::vec3 PreviousPosition;
};

}

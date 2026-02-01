#pragma once

#include <glm/glm.hpp>

namespace VoxelEngine
{

struct TransformComponent
{
    glm::vec3 Position = glm::vec3(0.0f);
    float_t RotationAngle = 0.0f;
    glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);
    glm::vec3 PreviousPosition = glm::vec3(0.0f);

    glm::mat4 World = glm::mat4(1.0f);

    TransformComponent operator+=(const TransformComponent other) const
    {
        TransformComponent result;

        result.Position = Position + other.Position;
        result.RotationAngle = RotationAngle + other.RotationAngle;
        result.Scale = Scale + other.Scale;
        result.PreviousPosition = Position;

        if (other.RotationAxis != glm::vec3(0.0f))
            result.RotationAxis = RotationAxis + other.RotationAxis;
        else
            result.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        return result;
    }
};

}

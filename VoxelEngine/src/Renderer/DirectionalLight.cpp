//
// Created by RadU on 1/2/2026.
//

#include "DirectionalLight.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace VoxelEngine
{

DirectionalLight::DirectionalLight(const glm::vec3 direction, const glm::vec3 color)
    : Direction(glm::normalize(direction)), LightColor(color)
{
}

glm::mat4 DirectionalLight::GetLightSpaceTransform(const glm::vec3 position) const
{
    constexpr float nearPlane = 0.5f;
    constexpr float farPlane = 100.0f;

    const glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, nearPlane, farPlane);
    const glm::mat4 lightView = glm::lookAt(position, position + Direction, glm::vec3(0.0f, 1.0f, 0.0f));

    return lightProjection * lightView;
}

}

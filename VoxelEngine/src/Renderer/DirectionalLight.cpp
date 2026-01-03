//
// Created by RadU on 1/2/2026.
//

#include "DirectionalLight.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../Config.hpp"

namespace VoxelEngine
{

DirectionalLight::DirectionalLight(const glm::vec3 direction, const float_t intensity, const glm::vec3 color)
    : Direction(glm::normalize(direction)), LightColor(color), LightIntensity(intensity)
{
}

glm::mat4 DirectionalLight::GetLightSpaceTransform(const glm::vec3 cameraPosition) const
{
    const float nearPlane = Config::ShadowNearPlane;
    const float farPlane = Config::ShadowFarPlane;
    const float size = Config::ShadowFrustumSize;

    const glm::vec3 position = cameraPosition + glm::vec3(0.0f, 50.0f, 0.0f);
    const glm::vec3 center = position + Direction;

    glm::vec3 up;
    if (abs(Direction.y) > 0.99f)
        up = glm::vec3(0, 0, 1);
    else
        up = glm::vec3(0, 1, 0);

    const glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);
    const glm::mat4 lightView = glm::lookAt(position, center, up);

    return lightProjection * lightView;
}

}

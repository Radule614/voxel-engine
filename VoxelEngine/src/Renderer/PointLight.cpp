//
// Created by RadU on 1/1/2026.
//

#include "PointLight.hpp"
#include "../Config.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace VoxelEngine
{

PointLight::PointLight(const glm::vec3 position, const glm::vec3 color)
    : Position(position), LightColor(color), DepthCubeMap(0)
{
}

std::vector<glm::mat4> PointLight::CalculateShadowTransforms() const
{
    static float_t aspect = (float_t) Config::PointShadowWidth / (float_t) Config::PointShadowHeight;
    static glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f),
                                                         aspect,
                                                         Config::PointShadowNearPlane,
                                                         Config::PointShadowFarPlane);

    std::vector<glm::mat4> shadowTransforms{};
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(1.0, 0.0, 0.0),
                                                              glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(-1.0, 0.0, 0.0),
                                                              glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(0.0, 1.0, 0.0),
                                                              glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(0.0, -1.0, 0.0),
                                                              glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(0.0, 0.0, 1.0),
                                                              glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(Position,
                                                              Position + glm::vec3(0.0, 0.0, -1.0),
                                                              glm::vec3(0.0, -1.0, 0.0)));

    return shadowTransforms;
}

}

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

    PointLight(glm::vec3 position, glm::vec3 color);
    ~PointLight();

    PointLight(const PointLight&) = delete;
    PointLight(PointLight&& other) noexcept;

    PointLight& operator=(const PointLight&) = delete;
    PointLight& operator=(PointLight&& other) noexcept;

    GLuint GetDepthCubeMap() const;
    std::vector<glm::mat4> CalculateShadowTransforms() const;

private:
    GLuint m_DepthCubeMap = 0;
};

}

//
// Created by RadU on 1/1/2026.
//

#include "PointLight.hpp"
#include "../Config.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace VoxelEngine
{

static void CreateDepthCubeMap(GLuint* depthCubeMap);

PointLight::PointLight(const glm::vec3 position, const glm::vec3 color) : Position(position), LightColor(color)
{
    CreateDepthCubeMap(&m_DepthCubeMap);
}

PointLight::~PointLight()
{
    if (m_DepthCubeMap != 0)
    {
        glDeleteTextures(1, &m_DepthCubeMap);
        m_DepthCubeMap = 0;
    }
}

PointLight::PointLight(PointLight&& other) noexcept
    : Position(other.Position), LightColor(other.LightColor), m_DepthCubeMap(other.m_DepthCubeMap)
{
    other.m_DepthCubeMap = 0;
}

PointLight& PointLight::operator=(PointLight&& other) noexcept
{
    {
        if (this != &other)
        {
            glDeleteTextures(1, &m_DepthCubeMap);
            m_DepthCubeMap = other.m_DepthCubeMap;
            other.m_DepthCubeMap = 0;

            Position = other.Position;
            LightColor = other.LightColor;
        }
        return *this;
    }
}

GLuint PointLight::GetDepthCubeMap() const { return m_DepthCubeMap; }

std::vector<glm::mat4> PointLight::CalculateShadowTransforms() const
{
    static float_t aspect = (float_t) Config::ShadowWidth / (float_t) Config::ShadowHeight;
    static glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),
                                                   aspect,
                                                   Config::ShadowNearPlane,
                                                   Config::ShadowFarPlane);

    std::vector<glm::mat4> shadowTransforms{};
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(-1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(0.0, 1.0, 0.0),
                                                        glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(0.0, -1.0, 0.0),
                                                        glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(0.0, 0.0, 1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(Position,
                                                        Position + glm::vec3(0.0, 0.0, -1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));

    return shadowTransforms;
}

static void CreateDepthCubeMap(GLuint* depthCubeMap)
{
    glGenTextures(1, depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubeMap);

    for (uint32_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_DEPTH_COMPONENT,
                     Config::ShadowWidth,
                     Config::ShadowHeight,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

}

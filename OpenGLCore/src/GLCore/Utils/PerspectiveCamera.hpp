#pragma once

#include <glm/glm.hpp>

namespace GLCore::Utils
{
class PerspectiveCamera
{
public:
    PerspectiveCamera(float_t fov, float_t aspectRatio);

    void SetProjection(float_t fov, float_t aspectRatio);

    const glm::vec3 &GetPosition() const
    {
        return m_Position;
    }
    void SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        RecalculateViewMatrix();
    }

    const glm::vec3 GetFront() const
    {
        return m_Front;
    }

    void SetFront(glm::vec3 &front)
    {
        m_Front = front;
        RecalculateViewMatrix();
    }

    const glm::vec3 &GetUp() const
    {
        return m_Up;
    }

    const glm::vec3 &GetRight() const
    {
        return m_Right;
    }

    const glm::mat4 &GetProjectionMatrix() const
    {
        return m_ProjectionMatrix;
    }
    const glm::mat4 &GetViewMatrix() const
    {
        return m_ViewMatrix;
    }
    const glm::mat4 &GetViewProjectionMatrix() const
    {
        return m_ViewProjectionMatrix;
    }

private:
    void RecalculateViewMatrix();

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    glm::vec3 m_Position = {0.0f, 0.0f, 5.0f};

    glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 m_Right;
    glm::vec3 m_Up;
};
}

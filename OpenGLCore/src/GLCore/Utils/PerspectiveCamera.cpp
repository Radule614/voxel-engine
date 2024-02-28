#include "PerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace GLCore::Utils
{
PerspectiveCamera::PerspectiveCamera(float_t fov, float_t aspectRatio)
    : m_ProjectionMatrix(glm::perspective(fov, aspectRatio, 0.01f, 200.0f))
{
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::SetProjection(float_t fov, float_t aspectRatio)
{
    m_ProjectionMatrix = glm::perspective(fov, aspectRatio, 0.01f, 200.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateViewMatrix()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

    m_ViewMatrix = glm::inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
} // namespace GLCore::Utils

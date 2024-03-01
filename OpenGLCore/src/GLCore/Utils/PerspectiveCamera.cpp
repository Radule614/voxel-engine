#include "PerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace GLCore::Utils
{
PerspectiveCamera::PerspectiveCamera(float_t fov, float_t aspectRatio)
    : m_ProjectionMatrix(glm::perspective(fov, aspectRatio, 0.01f, 200.0f))
{
    RecalculateViewMatrix();
}

void PerspectiveCamera::SetProjection(float_t fov, float_t aspectRatio)
{
    m_ProjectionMatrix = glm::perspective(fov, aspectRatio, 0.01f, 200.0f);
    RecalculateViewMatrix();
}

void PerspectiveCamera::RecalculateViewMatrix()
{
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace GLCore::Utils

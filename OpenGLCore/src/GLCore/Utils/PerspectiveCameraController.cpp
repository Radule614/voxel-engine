#include "glpch.hpp"
#include "PerspectiveCameraController.hpp"

#include "GLCore/Core/Input.hpp"
#include "GLCore/Core/KeyCodes.hpp"

namespace GLCore::Utils
{
PerspectiveCameraController::PerspectiveCameraController(float_t fov, float_t aspectRatio)
    : m_AspectRatio(aspectRatio), m_Camera(fov, aspectRatio), m_Fov(fov)
{

}

void PerspectiveCameraController::OnUpdate(Timestep ts)
{
    glm::vec3 frontDir = glm::vec3();
    glm::vec3 rightDir = glm::vec3();
    if (Input::IsKeyPressed(HZ_KEY_A))
        rightDir = -glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp()));
    else if (Input::IsKeyPressed(HZ_KEY_D))
        rightDir = glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp()));

    if (Input::IsKeyPressed(HZ_KEY_W))
        frontDir = m_Camera.GetFront();
    else if (Input::IsKeyPressed(HZ_KEY_S))
        frontDir = -m_Camera.GetFront();

    glm::vec3 direction;
    if (frontDir != glm::vec3(0) || rightDir != glm::vec3(0))
    {
        direction = glm::normalize(frontDir + rightDir);
        m_Camera.SetPosition(m_Camera.GetPosition() + (float_t)ts * m_CameraTranslationSpeed * direction);
    }
}

void PerspectiveCameraController::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
    dispatcher.Dispatch<MouseMovedEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
}

bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent &e)
{
    //TODO
    return false;
}

bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent &e)
{
    m_AspectRatio = (float_t)e.GetWidth() / (float_t)e.GetHeight();
    m_Camera.SetProjection(m_Fov, m_AspectRatio);
    return false;
}

bool PerspectiveCameraController::OnMouseMoved(MouseMovedEvent &e)
{
    float xoffset = (e.GetX() - m_LastX) * m_Sensitivity;
    float yoffset = (m_LastY - e.GetY()) * m_Sensitivity;
    m_LastX = e.GetX();
    m_LastY = e.GetY();
    m_Yaw += xoffset;
    m_Pitch += yoffset;
    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    direction.y = sin(glm::radians(m_Pitch));
    direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    glm::vec3 front = glm::normalize(direction);
    m_Camera.SetFront(front);

    return false;
}

} // namespace GLCore::Utils

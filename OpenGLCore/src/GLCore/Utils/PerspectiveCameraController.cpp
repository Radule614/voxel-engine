#include "glpch.hpp"
#include "PerspectiveCameraController.hpp"

#include "GLCore/Core/Input.hpp"
#include "GLCore/Core/KeyCodes.hpp"

namespace GLCore::Utils
{
PerspectiveCameraController::PerspectiveCameraController(float_t fov, float_t aspectRatio, float_t speed)
    : m_AspectRatio(aspectRatio),
      m_Camera(fov, aspectRatio),
      m_Fov(fov),
      m_CameraTranslationSpeed(speed)
{
}

PerspectiveCameraController::PerspectiveCameraController() : PerspectiveCameraController(45.0f, 16.0f / 9.0f, 150.0f)
{
}

void PerspectiveCameraController::OnUpdate(const Timestep ts)
{
    if (!m_FreeFly)
        return;
    const auto oldPos = GetCamera().GetPosition();
    const auto direction = CalculateMovementDirection();
    GetCamera().SetPosition(oldPos + direction * m_CameraTranslationSpeed * static_cast<float_t>(ts));
}

glm::vec3 PerspectiveCameraController::CalculateMovementDirection() const
{
    auto frontDir = glm::vec3();
    auto rightDir = glm::vec3();

    if (Input::IsKeyPressed(VE_KEY_A))
        rightDir = -glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp()));
    else if (Input::IsKeyPressed(VE_KEY_D))
        rightDir = glm::normalize(glm::cross(m_Camera.GetFront(), m_Camera.GetUp()));

    if (Input::IsKeyPressed(VE_KEY_W))
        frontDir = m_Camera.GetFront();
    else if (Input::IsKeyPressed(VE_KEY_S))
        frontDir = -m_Camera.GetFront();

    auto direction = frontDir + rightDir;

    if (!m_FreeFly)
        direction.y = 0.0f;

    return direction == glm::vec3(0.0f)
               ? glm::vec3(0.0f)
               : glm::normalize(direction);
}

void PerspectiveCameraController::SetFreeFly(bool value) { m_FreeFly = value; }

bool PerspectiveCameraController::IsFreeFly() const { return m_FreeFly; }

void PerspectiveCameraController::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<StateUnpauseEvent>(
        [&](const StateUnpauseEvent&) {
            m_First = true;
            return false;
        });
    dispatcher.Dispatch<MouseScrolledEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
    dispatcher.Dispatch<MouseMovedEvent>(GLCORE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
}

bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    //TODO
    return false;
}

bool PerspectiveCameraController::OnWindowResized(const WindowResizeEvent& e)
{
    m_AspectRatio = static_cast<float_t>(e.GetWidth()) / static_cast<float_t>(e.GetHeight());
    m_Camera.SetProjection(m_Fov, m_AspectRatio);
    return false;
}

bool PerspectiveCameraController::OnMouseMoved(const MouseMovedEvent& e)
{
    if (m_First)
    {
        m_LastX = e.GetX();
        m_LastY = e.GetY();
        const glm::vec3 front = m_Camera.GetFront();
        m_Yaw = glm::degrees(glm::atan(front.z, front.x));
        m_Pitch = glm::degrees(glm::asin(front.y));
        m_First = false;
    }
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

PerspectiveCamera& PerspectiveCameraController::GetCamera() { return m_Camera; }

const PerspectiveCamera& PerspectiveCameraController::GetCamera() const { return m_Camera; }

float_t PerspectiveCameraController::GetZoomLevel() const { return m_ZoomLevel; }

void PerspectiveCameraController::SetZoomLevel(const float_t level) { m_ZoomLevel = level; }

float_t PerspectiveCameraController::GetFov() const { return m_Fov; }

}

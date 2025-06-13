#pragma once

#include "PerspectiveCamera.hpp"
#include "GLCore/Core/Timestep.hpp"

#include "GLCore/Events/ApplicationEvent.hpp"
#include "GLCore/Events/MouseEvent.hpp"
#include "GLCore/Events/StateEvent.hpp"

namespace GLCore::Utils
{
class PerspectiveCameraController
{
public:
    PerspectiveCameraController();
    PerspectiveCameraController(float_t fov, float_t aspectRatio, float_t speed);

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

    PerspectiveCamera& GetCamera();
    const PerspectiveCamera& GetCamera() const;
    float_t GetZoomLevel() const;
    void SetZoomLevel(float_t level);
    float_t GetFov() const;
    glm::vec3 CalculateMovementVector(Timestep ts) const;

private:
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(const WindowResizeEvent& e);
    bool OnMouseMoved(const MouseMovedEvent& e);

private:
    float_t m_AspectRatio;
    float_t m_ZoomLevel = 1.0f;
    PerspectiveCamera m_Camera;
    float_t m_Fov = 1.0f;
    float_t m_CameraTranslationSpeed = 20.0f;
    float_t m_Sensitivity = 0.05f;
    float_t m_LastX = 0.0f;
    float_t m_LastY = 0.0f;
    float_t m_Yaw = 0.0f;
    float_t m_Pitch = 0.0f;

    bool m_First = true;
    bool m_FreeFly;
};
}

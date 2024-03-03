#pragma once

#include "PerspectiveCamera.hpp"
#include "GLCore/Core/Timestep.hpp"

#include "GLCore/Events/ApplicationEvent.hpp"
#include "GLCore/Events/MouseEvent.hpp"

namespace GLCore::Utils
{
class PerspectiveCameraController
{
public:
    PerspectiveCameraController(float_t fov, float_t aspectRatio);

    void OnUpdate(Timestep ts);
    void OnEvent(Event &e);

    PerspectiveCamera &GetCamera()
    {
        return m_Camera;
    }
    const PerspectiveCamera &GetCamera() const
    {
        return m_Camera;
    }

    float_t GetZoomLevel() const
    {
        return m_ZoomLevel;
    }
    void SetZoomLevel(float_t level)
    {
        m_ZoomLevel = level;
    }

    float_t GetFov() const
    {
        return m_Fov;
    }

private:
    bool OnMouseScrolled(MouseScrolledEvent &e);
    bool OnWindowResized(WindowResizeEvent &e);
    bool OnMouseMoved(MouseMovedEvent &e);

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
};
} // namespace GLCore::Utils

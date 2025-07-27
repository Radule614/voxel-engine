#include "UserInterface.hpp"

#include "GLCore/Core/Input.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

UserInterface::UserInterface(EngineState& state) : m_State(state)
{
}

UserInterface::~UserInterface() = default;

void UserInterface::OnAttach() { m_State.Application->GetWindow().CaptureMouse(true); }

void UserInterface::OnEvent(GLCore::Event& event)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnEvent(event);
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](KeyPressedEvent& e) {
            if (e.GetKeyCode() == VE_KEY_ESCAPE)
            {
                m_State.MenuActive = !m_State.MenuActive;
                if (!m_State.MenuActive)
                {
                    m_State.Application->GetWindow().CaptureMouse(true);
                    StateUnpauseEvent event;
                    m_State.Application->RaiseEvent(event);
                }
                else
                {
                    m_State.Application->GetWindow().CaptureMouse(false);
                    StatePauseEvent event;
                    m_State.Application->RaiseEvent(event);
                }
            }
            return false;
        });
}

void UserInterface::OnImGuiRender()
{
}

void UserInterface::OnUpdate(Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnUpdate(ts);
}

}

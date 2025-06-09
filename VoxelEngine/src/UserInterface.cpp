#include "UserInterface.hpp"

#include "GLCore/Core/Input.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

UserInterface::UserInterface(EngineState& state) : m_State(state)
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::OnAttach()
{
    m_State.Application->GetWindow().CaptureMouse(true);
}

void UserInterface::OnEvent(GLCore::Event& event)
{
    if (!m_State.MenuActive)
        m_State.CameraController.OnEvent(event);
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](KeyPressedEvent& e) {
            if (e.GetKeyCode() == HZ_KEY_ESCAPE)
            {
                m_State.MenuActive = !m_State.MenuActive;
                if (!m_State.MenuActive)
                {
                    m_State.Application->GetWindow().CaptureMouse(true);
                    StateUnpauseEvent event;
                    m_State.Application->RaiseEvent(event);
                } else
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
    if (!m_State.MenuActive)
        return;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoMove;
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(300.0, 400.0));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Test", nullptr, windowFlags);
    ImGui::Text("Hello, World!");
    if (ImGui::Button("Click Me"))
        ImGui::Text("Button was clicked!");
    ImGui::End();
}

void UserInterface::OnUpdate(GLCore::Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController.OnUpdate(ts);
}

}

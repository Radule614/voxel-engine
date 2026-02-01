#include "UserInterface.hpp"

#include "imgui.h"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

UserInterface::UserInterface(EngineState& state) : m_State(state)
{
}

UserInterface::~UserInterface() = default;

void UserInterface::OnAttach() { m_State.Application->GetWindow().CaptureMouse(true); }

void UserInterface::OnEvent(Event& event)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);

    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) {
            if (e.GetKeyCode() == VE_KEY_ESCAPE)
            {
                m_State.MenuActive = !m_State.MenuActive;

                if (!m_State.MenuActive)
                {
                    m_State.Application->GetWindow().CaptureMouse(true);
                    StateUnpauseEvent newEvent;
                    m_State.Application->RaiseEvent(newEvent);
                }
                else
                {
                    m_State.Application->GetWindow().CaptureMouse(false);
                    StatePauseEvent newEvent;
                    m_State.Application->RaiseEvent(newEvent);
                }
            }

            return false;
        });
}

void UserInterface::OnImGuiRender()
{
    if (!m_State.MenuActive)
        return;

    DrawEntityViewer();
}

void UserInterface::OnUpdate(Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnUpdate(ts);
}

void UserInterface::DrawEntityViewer() const
{
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowSize(ImVec2(350.0, m_State.Application->GetWindow().GetHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Entity Viewer", nullptr, windowFlags);
    ImGui::Text("Entity Tree");

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto view = registry.view<TransformComponent>();
    for (const auto& entity: view)
    {
        auto& transform = view.get<TransformComponent>(entity);

        std::string position = VecToString(transform.Position);
        ImGui::Text(std::format("Transform: {0}", position).c_str());
    }

    ImGui::End();
}

}

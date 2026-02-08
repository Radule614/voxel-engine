#include "UserInterface.hpp"

#include "imgui.h"
#include "Ecs/Ecs.hpp"
#include "Ecs/Scene.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static void DisplayEntity(entt::entity entity);

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
    DrawEntityViewer();
}

void UserInterface::OnUpdate(Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnUpdate(ts);
}

void UserInterface::DrawEntityViewer() const
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowSize(ImVec2(400.0, m_State.Application->GetWindow().GetHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Entity Viewer", nullptr, windowFlags);
    ImGui::Text("Entity Tree");
    ImGui::Separator();

    const auto view = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto& entity: view)
        DisplayEntity(entity);

    ImGui::End();
}

static void DisplayEntity(const entt::entity entity)
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::PushID((int32_t) entity);

    std::string entityName = "Entity";

    const MetadataComponent* metadata = registry.try_get<MetadataComponent>(entity);

    if (metadata != nullptr)
        entityName = metadata->Name;

    if (ImGui::TreeNodeEx(entityName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        const TransformComponent* transform = registry.try_get<TransformComponent>(entity);

        if (transform != nullptr)
        {
            std::string position = VecToString(transform->LocalPosition);
            ImGui::Text(std::format("Transform: {0}", position).c_str());
        }

        const ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);

        if (children != nullptr)
        {
            ImGui::Text("Children:");

            for (const auto& childEntity: children->Entities)
                DisplayEntity(childEntity);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

}

#include "UserInterface.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/ParentComponent.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Utils/Utils.hpp"
#include "Ecs/ComponentGui.hpp"
#include "Ecs/Components/AnimationComponent.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Ecs/Components/ScriptComponent.hpp"
#include "Ecs/Components/SkinComponent.hpp"

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

static void SetupDockspace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("Dockspace");

    if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr)
    {
        ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_NoUndocking |
                                   ImGuiDockNodeFlags_NoResize |
                                   ImGuiDockNodeFlags_HiddenTabBar |
                                   ImGuiDockNodeFlags_DockSpace;

        ImGui::DockBuilderAddNode(dockspaceId, flags);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
        ImGui::DockBuilderSetNodePos(dockspaceId, ImVec2(0, 0));

        ImGuiID dockIdLeft;
        ImGuiID dockIdMain = dockspaceId;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Left, 0.2f, &dockIdLeft, &dockIdMain);

        ImGuiID dockIdRight;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Right, 0.2f, &dockIdRight, &dockIdMain);

        ImGuiID dockIdUp;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Up, 60.0f / viewport->Size.y, &dockIdUp, &dockIdMain);

        ImGuiID dockIdDown;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Down, 0.2f, &dockIdDown, &dockIdMain);

        ImGui::DockBuilderDockWindow("Viewport", dockIdMain);
        ImGui::DockBuilderDockWindow("Scene Explorer", dockIdLeft);
        ImGui::DockBuilderDockWindow("Entity Component", dockIdRight);
        ImGui::DockBuilderDockWindow("Voxel Layer", dockIdRight);
        ImGui::DockBuilderDockWindow("Scene Controls", dockIdUp);
        ImGui::DockBuilderDockWindow("Resource Explorer", dockIdDown);

        ImGui::DockBuilderFinish(dockspaceId);
    }

    ImGui::DockSpaceOverViewport(dockspaceId, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Scene Controls");
    ImGui::End();

    ImGui::Begin("Resource Explorer");
    ImGui::End();
}

void UserInterface::OnImGuiRender()
{
    SetupDockspace();

    DrawSceneViewer();
    DrawComponentViewer();
}

void UserInterface::OnUpdate(const Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnUpdate(ts);
}

void UserInterface::DrawSceneViewer()
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::Begin("Scene Explorer");

    const auto view = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto& entity: view)
        DrawNode(entity);

    ImGui::End();
}

void UserInterface::DrawNode(const entt::entity entity)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::PushID((int32_t) entity);

    const MetadataComponent* metadata = registry.try_get<MetadataComponent>(entity);
    const ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);

    std::string nodeName = "Node";
    if (metadata != nullptr)
        nodeName += std::format(": {}", metadata->Name);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (entity == m_SelectedEntity)
        flags |= ImGuiTreeNodeFlags_Selected;

    if (children == nullptr || children->Entities.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    const bool opened = ImGui::TreeNodeEx((void*) entity, flags, "%s", nodeName.c_str());

    if (ImGui::IsItemClicked())
        m_SelectedEntity = entity;

    if (opened)
    {
        if (children != nullptr)
        {
            for (const auto& childEntity: children->Entities)
                DrawNode(childEntity);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

template<typename T>
void DrawComponent(entt::registry& registry, const entt::entity entity)
{
    auto* component = dynamic_cast<ComponentGui*>(registry.try_get<T>(entity));
    if (component == nullptr)
        return;

    const auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx(component->GetName().c_str(), flags))
    {
        component->DrawGui();

        ImGui::TreePop();
    }
}

void UserInterface::DrawComponentViewer() const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::Begin("Entity Component");

    if (m_SelectedEntity == entt::null)
    {
        ImGui::End();

        return;
    }

    DrawComponent<TransformComponent>(registry, m_SelectedEntity);
    DrawComponent<MeshComponent>(registry, m_SelectedEntity);
    DrawComponent<ColliderComponent>(registry, m_SelectedEntity);
    DrawComponent<AnimationComponent>(registry, m_SelectedEntity);
    DrawComponent<SkinComponent>(registry, m_SelectedEntity);
    DrawComponent<ScriptComponent>(registry, m_SelectedEntity);

    ImGui::End();
}

}

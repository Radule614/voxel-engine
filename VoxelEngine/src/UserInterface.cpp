#include "UserInterface.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "Ecs/Ecs.hpp"
#include "Ecs/Scene.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static void DrawSceneViewer();
static void DrawNode(entt::entity entity);

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
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("Dockspace");

    ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoResize |
                               ImGuiDockNodeFlags_HiddenTabBar;

    ImGui::DockBuilderAddNode(dockspaceId, flags);
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
    ImGui::DockBuilderSetNodePos(dockspaceId, ImVec2(0, 0));

    ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);
    ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.2f, nullptr, &dockspaceId);
    ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Up, 0.2f, nullptr, &dockspaceId);
    ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.2f, nullptr, &dockspaceId);

    ImGui::SetNextWindowDockID(dockLeft, ImGuiCond_Always);
    ImGui::Begin("Left Panel");
    ImGui::End();
    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Always);
    ImGui::Begin("Right Panel");
    ImGui::End();
    ImGui::SetNextWindowDockID(dockTop, ImGuiCond_Always);
    ImGui::Begin("Top Panel");
    ImGui::Text("123");
    ImGui::End();
    ImGui::SetNextWindowDockID(dockBottom, ImGuiCond_Always);
    ImGui::Begin("Bottom Panel");
    ImGui::End();
    ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_Always);
    ImGui::Begin("Viewport");
    ImGui::End();

    ImGui::DockBuilderFinish(dockspaceId);
}

void UserInterface::OnImGuiRender()
{
    static bool dockspaceInitialized = false;

    if (!dockspaceInitialized)
    {
        dockspaceInitialized = true;

        SetupDockspace();
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpaceWindow", nullptr, hostFlags);
    ImGui::PopStyleVar(2);

    ImGui::End();

    ImGuiID dockspaceId = ImGui::GetID("Dockspace");
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
    ImGui::DockBuilderSetNodePos(dockspaceId, ImVec2(0, 0));

    DrawSceneViewer();
}

void UserInterface::OnUpdate(Timestep ts)
{
    if (!m_State.MenuActive)
        m_State.CameraController->OnUpdate(ts);
}

void DrawSceneViewer()
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::Begin("Left Panel");
    ImGui::Text("Entity Tree");
    ImGui::Separator();

    const auto view = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto& entity: view)
        DrawNode(entity);

    ImGui::End();
}

static void DrawNode(const entt::entity entity)
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    ImGui::PushID((int32_t) entity);

    std::string nodeName = "Node";

    const MetadataComponent* metadata = registry.try_get<MetadataComponent>(entity);

    if (metadata != nullptr)
        nodeName += std::format(": {}", metadata->Name);

    if (ImGui::TreeNodeEx(nodeName.c_str()))
    {
        TransformComponent* transform = registry.try_get<TransformComponent>(entity);
        if (transform != nullptr)
        {
            ImGui::Columns(2);
            ImGui::Dummy(ImVec2(0.0f, 1.0f));
            ImGui::SetColumnWidth(0, 80);

            ImGui::Text("Transform");

            ImGui::NextColumn();
            ImGui::DragFloat3(ImGui::GetVersion(), &transform->LocalPosition.x, 0.1f);

            ImGui::Columns(1);
        }

        const MeshComponent* mesh = registry.try_get<MeshComponent>(entity);
        if (mesh != nullptr)
        {
        }

        const ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);
        if (children != nullptr)
        {
            for (const auto& childEntity: children->Entities)
                DrawNode(childEntity);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

}

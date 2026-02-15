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

void UserInterface::OnImGuiRender()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    constexpr ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpaceWindow", nullptr, hostFlags);
    ImGui::PopStyleVar(2);

    const ImGuiID dockspaceId = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspaceId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_NoTabBar);
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
    ImGui::DockBuilderSetNodePos(dockspaceId, ImVec2(0, 0));

    ImGui::End();

    ImGui::Begin("Left Panel");
    ImGui::End();
    ImGui::Begin("Right Panel");
    ImGui::End();
    ImGui::Begin("Top Panel");
    ImGui::End();
    ImGui::Begin("Bottom Panel");
    ImGui::End();
    ImGui::Begin("Viewport");
    ImGui::End();

    ImGuiDockNode* dockspaceNode = ImGui::DockBuilderGetNode(dockspaceId);

    if (!dockspaceNode->ChildNodes[0])
    {
        const ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockspaceId,
                                                               ImGuiDir_Left,
                                                               0.2f,
                                                               nullptr,
                                                               (ImGuiID*) &dockspaceId);
        const ImGuiID dockIdRight = ImGui::DockBuilderSplitNode(dockspaceId,
                                                                ImGuiDir_Right,
                                                                0.2f,
                                                                nullptr,
                                                                (ImGuiID*) &dockspaceId);

        const ImGuiID dockIdUp = ImGui::DockBuilderSplitNode(dockspaceId,
                                                             ImGuiDir_Up,
                                                             40.0f / viewport->Size.y,
                                                             nullptr,
                                                             (ImGuiID*) &dockspaceId);

        const ImGuiID dockIdDown = ImGui::DockBuilderSplitNode(dockspaceId,
                                                               ImGuiDir_Down,
                                                               0.2f,
                                                               nullptr,
                                                               (ImGuiID*) &dockspaceId);

        ImGui::DockBuilderDockWindow("Left Panel", dockIdLeft);
        ImGui::DockBuilderDockWindow("Right Panel", dockIdRight);
        ImGui::DockBuilderDockWindow("Top Panel", dockIdUp);
        ImGui::DockBuilderDockWindow("Bottom Panel", dockIdDown);
        ImGui::DockBuilderDockWindow("Viewport", dockspaceId);

        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockIdLeft))
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockIdRight))
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockIdUp))
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockIdDown))
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceId))
            node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        ImGui::DockBuilderFinish(dockspaceId);
    }

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
        const TransformComponent* transform = registry.try_get<TransformComponent>(entity);
        if (transform != nullptr)
        {
            std::string position = VecToString(transform->LocalPosition);
            ImGui::Text(std::format("Transform: {0}", position).c_str());
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

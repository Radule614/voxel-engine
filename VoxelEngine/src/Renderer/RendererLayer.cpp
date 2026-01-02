//
// Created by RadU on 12/20/2025.
//

#include "RendererLayer.hpp"

#include "Renderer.hpp"
#include "../Ecs/Ecs.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

RendererLayer::RendererLayer(EngineState& state) : m_State(state), m_Renderer(state.Application->GetWindow())
{
}

void RendererLayer::OnAttach()
{
    EnableGLDebugging();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Renderer.Init();
}

void RendererLayer::OnUpdate(const Timestep ts)
{
    m_Renderer.RenderScene(m_State.CameraController->GetCamera());

    if (m_AccumulatedTime > 0.5f)
    {
        m_Fps = 1.0f / ts;

        m_AccumulatedTime = 0.0f;
    }
    m_AccumulatedTime += ts;
}

void RendererLayer::OnImGuiRender()
{
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowSize(ImVec2(500.0, 300.0));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Renderer", nullptr, windowFlags);
    ImGui::Text("Renderer");

    ImGui::Text("Fps: %.1f", m_Fps);

    ImGui::End();

    ImGui::Begin("Texture Debug");

    ImGui::Image(
        (ImTextureID) (intptr_t) m_Renderer.m_DepthMap,
        ImVec2(512, 512),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
}

}

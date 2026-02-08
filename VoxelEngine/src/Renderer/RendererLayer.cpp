//
// Created by RadU on 12/20/2025.
//

#include "RendererLayer.hpp"

#include "Renderer.hpp"
#include "../Ecs/Ecs.hpp"
#include "Target/ScreenRenderTarget.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static void DrawDepthMap(GLuint depthMap);

RendererLayer::RendererLayer(EngineState& state) : m_State(state)
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
    static RenderTarget* renderTarget = new ScreenRenderTarget(m_State.Application->GetWindow().GetWidth(),
                                                               m_State.Application->GetWindow().GetHeight());

    m_Renderer.RenderScene(m_State.CameraController->GetCamera(), *renderTarget);

    if (m_AccumulatedTime > 0.5f)
    {
        m_Fps = 1.0f / ts;

        m_State.Application->AppendToWindowTitle(std::format("Fps: {0}", m_Fps));

        m_AccumulatedTime = 0.0f;
    }
    m_AccumulatedTime += ts;
}

void RendererLayer::OnImGuiRender()
{
    // DrawDepthMap(m_Renderer.m_DepthMap);
}

static void DrawDepthMap(const GLuint depthMap)
{
    ImGui::Begin("Directional Light Depth Texture");

    ImGui::Image(
        (ImTextureID) (intptr_t) depthMap,
        ImVec2(256, 256),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
}

}

//
// Created by RadU on 12/20/2025.
//

#include "RendererLayer.hpp"

#include "Renderer.hpp"
#include "../Ecs/Ecs.hpp"
#include "Target/ScreenRenderTarget.hpp"
#include "Target/TextureRenderTarget.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static void DrawTextureInViewport(GLuint texture, int32_t width, int32_t height);

RendererLayer::RendererLayer(EngineState& state) : m_TextureRenderTarget(1280, 720), m_State(state)
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
    m_Renderer.RenderScene(m_State.CameraController->GetCamera(), m_TextureRenderTarget);

    if (m_AccumulatedTime > 0.5f)
    {
        m_Fps = 1.0f / ts;

        m_State.Application->AppendToWindowTitle(fmt::format("Fps: {0}", m_Fps));

        m_AccumulatedTime = 0.0f;
    }
    m_AccumulatedTime += ts;
}

void RendererLayer::OnImGuiRender()
{
    DrawTextureInViewport(m_TextureRenderTarget.GetTexture(),
                          m_TextureRenderTarget.GetWidth(),
                          m_TextureRenderTarget.GetHeight());

    ResizeTextureRenderTarget();
}

void RendererLayer::ResizeTextureRenderTarget()
{
    ImGui::Begin("Viewport");
    const ImVec2 viewportSize = ImGui::GetWindowSize();
    ImGui::End();

    const int32_t width = viewportSize.x;
    const int32_t height = viewportSize.y - 20;

    if (width != m_TextureRenderTarget.GetWidth() || height != m_TextureRenderTarget.GetHeight())
    {
        const float_t aspectRatio = width / (float_t) height;
        m_State.CameraController->GetCamera().SetProjection(45.0f, aspectRatio);

        m_TextureRenderTarget.Resize(width, height);
    }
}

static void DrawTextureInViewport(const GLuint texture, const int32_t width, const int32_t height)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImGui::Image(texture, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
    ImGui::PopStyleVar();
}

}

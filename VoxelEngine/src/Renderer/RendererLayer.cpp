//
// Created by RadU on 12/20/2025.
//

#include "RendererLayer.hpp"

#include "Renderer.hpp"

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
}

void RendererLayer::OnUpdate(Timestep ts) { m_Renderer.RenderScene(m_State.CameraController->GetCamera()); }

}

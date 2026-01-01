//
// Created by RadU on 12/20/2025.
//

#pragma once

#include "Renderer.hpp"
#include "../EngineState.hpp"

namespace VoxelEngine
{

class RendererLayer : public GLCore::Layer
{
public:
    explicit RendererLayer(EngineState& state);

    void OnAttach() override;
    void OnUpdate(GLCore::Timestep ts) override;
    void OnImGuiRender() override;

private:
    EngineState& m_State;
    Renderer m_Renderer;

    float_t m_Fps = 0.0f;
    float_t m_AccumulatedTime = 0.0f;
};

}

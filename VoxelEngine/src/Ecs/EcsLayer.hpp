//
// Created by RadU on 2/5/2026.
//

#pragma once

#include "../EngineState.hpp"

#include "GLCore/Core/Layer.hpp"

namespace VoxelEngine
{

class EcsLayer final : public GLCore::Layer
{
public:
    explicit EcsLayer(EngineState& state);

    void OnAttach() override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    EngineState& m_State;
};

}

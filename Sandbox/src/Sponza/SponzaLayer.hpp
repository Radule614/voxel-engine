//
// Created by RadU on 12/13/2025.
//

#pragma once

#include "EngineState.hpp"

namespace Sandbox
{

class SponzaLayer  : public GLCore::Layer
{
public:
    explicit SponzaLayer(VoxelEngine::EngineState& state);
    ~SponzaLayer() override;

    void OnAttach() override;

private:
    VoxelEngine::EngineState& m_State;
};

}

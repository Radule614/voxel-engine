#pragma once

#include "../EngineState.hpp"

namespace VoxelEngine
{

class PhysicsLayer : public GLCore::Layer
{
public:
    explicit PhysicsLayer(EngineState& state);

    void OnAttach() override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    EngineState& m_State;
};

}

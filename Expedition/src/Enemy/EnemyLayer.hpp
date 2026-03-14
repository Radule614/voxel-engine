#pragma once

#include <GLCore.hpp>
#include "EngineState.hpp"

namespace Expedition
{

class EnemyLayer : public GLCore::Layer
{
public:
    explicit EnemyLayer(VoxelEngine::EngineState& state);
    void OnAttach() override;

private:
    VoxelEngine::EngineState& m_State;
};

}

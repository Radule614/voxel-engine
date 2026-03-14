#pragma once
#include <GLCore.hpp>
#include "EngineState.hpp"

namespace Expedition
{
    class PlayerLayer : public GLCore::Layer {
    public:
        explicit PlayerLayer(VoxelEngine::EngineState& state);
        void OnAttach() override;
    private:
        VoxelEngine::EngineState& m_State;
    };
}

#pragma once
#include "VoxelEngineApp.hpp"

namespace Expedition
{
    class ExpeditionApp final : public VoxelEngine::VoxelEngineApp {
    protected:
        void Setup() override;
    };
}

//
// Created by RadU on 3/1/2026.
//

#pragma once

#include "Ecs/Components/ScriptComponent.hpp"

namespace VoxelEngine
{

struct MutantScript : Script
{
    explicit MutantScript();

    void OnEvent(GLCore::Event& event, ScriptContext context) override;
    void OnUpdate(GLCore::Timestep ts, ScriptContext context) override;
};

}

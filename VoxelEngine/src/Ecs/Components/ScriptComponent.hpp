//
// Created by RadU on 3/1/2026.
//

#pragma once

#include "entt.hpp"
#include "../ComponentGui.hpp"

namespace VoxelEngine
{

struct ScriptContext
{
    entt::entity Entity;
};

struct Script
{
    std::string Name;
    std::function<void(ScriptContext)> OnUpdate;
};

struct ScriptComponent : ComponentGui
{
    std::vector<Script> Scripts{};

    std::string GetName() override;
    void DrawGui() override;
};

}

#pragma once

#include "entt.hpp"
#include "Ecs/Components/ScriptComponent.hpp"

namespace Expedition
{

struct HealthBarScript : VoxelEngine::Script
{
    HealthBarScript(entt::entity rootEntity, entt::entity fillEntity, float barWidth);

    void OnUpdate(GLCore::Timestep ts, VoxelEngine::ScriptContext context) override;
    void OnEvent(GLCore::Event& event, VoxelEngine::ScriptContext context) override;
    void OnDetach(VoxelEngine::ScriptContext context) override;

private:
    entt::entity m_RootEntity;
    entt::entity m_FillEntity;
    float        m_BarWidth;
};

}

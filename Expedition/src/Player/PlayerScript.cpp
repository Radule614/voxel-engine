#include "PlayerScript.hpp"

#include "Ecs/Components/CharacterComponent.hpp"
#include "GLCore/Core/Input.hpp"

using namespace VoxelEngine;
using namespace GLCore;

namespace Expedition
{

PlayerScript::PlayerScript() : Script("Player Script")
{
}

void PlayerScript::OnUpdate(const Timestep ts, ScriptContext context)
{
    auto& registry = context.Registry;
    auto* charComp = registry.try_get<CharacterComponent>(context.Entity);
    if (!charComp)
        return;

    auto& controller = *charComp->Controller;

    if (m_Attacking)
    {
        m_AttackTimer -= ts.GetSeconds();
        if (m_AttackTimer <= 0.0f)
        {
            m_Attacking               = false;
            controller.m_CharacterSpeed = m_SavedSpeed;
        }
    }
    else if (Input::IsMouseButtonPressed(0))
    {
        m_Attacking               = true;
        m_AttackTimer             = m_AttackDuration;
        m_SavedSpeed              = controller.m_CharacterSpeed;
        controller.m_CharacterSpeed = 0.0f;
    }
}

void PlayerScript::OnEvent(Event& event, ScriptContext context)
{
}

void PlayerScript::OnDetach(ScriptContext context)
{
}

}

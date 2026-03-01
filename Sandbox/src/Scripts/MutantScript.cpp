//
// Created by RadU on 3/1/2026.
//

#include "MutantScript.hpp"

#include "Ecs/Components/AnimationComponent.hpp"
#include "GLCore/Events/MouseEvent.hpp"

using namespace GLCore;

namespace VoxelEngine
{

MutantScript::MutantScript() : Script("Mutant Script")
{
}

void MutantScript::OnEvent(Event& event, const ScriptContext context)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonReleasedEvent>(
        [&](const MouseButtonReleasedEvent& e) {
            auto& animations = context.Registry.get<AnimationComponent>(context.Entity).Animations;

            for (EntityAnimation& animation: animations)
            {
                animation.IsActive = false;

                if (animation.Name == "attack")
                {
                    animation.IsActive = true;
                    animation.ShouldRepeat = false;
                    animation.Time = 0.0f;
                }
            }

            return false;
        });
}

void MutantScript::OnUpdate(Timestep ts, ScriptContext context)
{
}

}

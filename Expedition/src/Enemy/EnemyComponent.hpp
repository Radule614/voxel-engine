#pragma once

#include "Physics/Character/CharacterBuilder.hpp"
#include "Jolt/Physics/Character/Character.h"

namespace Expedition
{

struct EnemyComponent
{
    std::unique_ptr<JPH::Character> Character;
    float VerticalVelocity = 0.0f;

    bool IsDying       = false;
    bool ReadyToRemove = false;

    explicit EnemyComponent(JPH::Character* character)
        : Character(character)
    {
    }
};

}

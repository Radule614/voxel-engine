//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../../Physics/PhysicsEngine.hpp"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

struct PlayerComponent
{
    PlayerComponent(JPH::CharacterVirtual* character) : Character(character)
    {
    }

    JPH::CharacterVirtual* Character;
};

}

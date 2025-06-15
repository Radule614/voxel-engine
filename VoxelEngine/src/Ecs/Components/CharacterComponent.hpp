//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../../Physics/PhysicsEngine.hpp"
#include "Jolt/Physics/Character/Character.h"

namespace VoxelEngine
{

struct CharacterComponent
{
    CharacterComponent(JPH::Character* character) : Character(character)
    {
    }

    JPH::Character* Character;
};

}

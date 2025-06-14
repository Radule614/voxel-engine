//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../../Physics/PhysicsEngine.hpp"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

struct CharacterComponent
{
    CharacterComponent(const std::shared_ptr<JPH::CharacterVirtual>& character) : Character(character)
    {
    }

    std::shared_ptr<JPH::CharacterVirtual> Character;
};

}

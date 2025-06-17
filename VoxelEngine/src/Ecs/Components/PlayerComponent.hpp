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
    PlayerComponent(std::unique_ptr<JPH::CharacterVirtual>& character) : Character(std::move(character))
    {
    }

    std::unique_ptr<JPH::CharacterVirtual> Character;
    glm::vec3 Velocity = glm::vec3(0.0f);
};

}

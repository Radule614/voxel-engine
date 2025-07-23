//
// Created by RadU on 7/22/2025.
//

#pragma once

#include "../PhysicsEngine.hpp"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

class CharacterController
{
public:
    CharacterController(std::unique_ptr<JPH::CharacterVirtual>&& character);

    void HandleInput(JPH::Vec3Arg inMovementDirection, bool inJump, float inDeltaTime);
    JPH::CharacterVirtual& GetCharacter() const;

public:
    bool m_InertiaEnabled = false;
    float_t m_CharacterSpeed = 10.0f;
    float_t m_JumpSpeed = 10.0f;
    float_t m_GravityStrength = 3.0f;

private:
    std::unique_ptr<JPH::CharacterVirtual> m_Character;

    bool m_AllowSliding = false;
    JPH::Vec3 m_DesiredVelocity = JPH::Vec3::sZero();
};

}

//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

class CharacterBuilder
{
public:
    CharacterBuilder();
    ~CharacterBuilder();

    CharacterBuilder& SetMaxSlopeAngle(const float_t& degrees);
    CharacterBuilder& SetPosition(const glm::vec3& position);
    CharacterBuilder& SetHeight(float_t height);
    CharacterBuilder& SetRadius(float_t radius);

    JPH::Character* BuildAndAdd() const;
    std::unique_ptr<JPH::CharacterVirtual> BuildAndAddVirtual() const;

private:
    float m_CharacterHeight = 2.0f;
    float m_CharacterRadius = 0.3f;
    float m_MaxSlopeAngleInDegrees = 15.0f; // TODO: Find the right value for this
    glm::vec3 m_Position = glm::vec3(0, 0, 0);
};

}

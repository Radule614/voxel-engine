//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

class CharacterBuilder
{
public:
    CharacterBuilder();
    ~CharacterBuilder();

    CharacterBuilder& SetShape(const JPH::ShapeRefC& shape);
    CharacterBuilder& SetMaxSlopeAngle(const float_t& degrees);
    CharacterBuilder& SetPosition(const glm::vec3& position);

    JPH::CharacterVirtual BuildAndAdd() const;

private:
    JPH::ShapeRefC m_Shape = nullptr;
    float m_MaxSlopeAngleInDegrees = 0.0f;
    glm::vec3 m_Position = glm::vec3(0, 0, 0);
};

}

//
// Created by RadU on 14-Jun-25.
//

#include "CharacterBuilder.hpp"

#include "JoltUtils.hpp"

using namespace JPH;

namespace VoxelEngine
{

CharacterBuilder::CharacterBuilder()
{
}

CharacterBuilder::~CharacterBuilder()
{
}

CharacterBuilder& CharacterBuilder::SetShape(const ShapeRefC& shape)
{
    m_Shape = shape;
    return *this;
}

CharacterBuilder& CharacterBuilder::SetMaxSlopeAngle(const float_t& degrees)
{
    m_MaxSlopeAngleInDegrees = degrees;
    return *this;
}

CharacterBuilder& CharacterBuilder::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    return *this;
}

Character* CharacterBuilder::BuildAndAdd() const
{
    PhysicsSystem* system = &PhysicsEngine::Instance().GetSystem();
    CharacterSettings settings;
    settings.mShape = m_Shape;
    settings.mMaxSlopeAngle = DegreesToRadians(45.0f);
    settings.mGravityFactor = 3.0f;
    const Vec3 position = JoltUtils::GlmToJoltVec3(m_Position);

    const auto character = new Character(&settings,
                                         position,
                                         Quat::sIdentity(),
                                         0,
                                         system);
    character->SetLayer(Layers::MOVING);
    character->AddToPhysicsSystem(EActivation::Activate);
    return character;
}

CharacterVirtual* CharacterBuilder::BuildAndAddVirtual() const
{
    PhysicsSystem* system = &PhysicsEngine::Instance().GetSystem();
    CharacterVirtualSettings settings;
    settings.mShape = m_Shape;
    settings.mMaxSlopeAngle = DegreesToRadians(45.0f);
    const Vec3 position = JoltUtils::GlmToJoltVec3(m_Position);

    const auto character = new CharacterVirtual(&settings,
                                         position,
                                         Quat::sIdentity(),
                                         system);
    return character;
}

}

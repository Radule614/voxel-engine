//
// Created by RadU on 14-Jun-25.
//

#include "CharacterBuilder.hpp"

#include "Jolt/Physics/Character/CharacterVirtual.h"
#include "../Utils/JoltUtils.hpp"
#include "../Utils/ShapeFactory.hpp"

using namespace JPH;

namespace VoxelEngine
{

CharacterBuilder::CharacterBuilder() = default;

CharacterBuilder::~CharacterBuilder() = default;

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

CharacterBuilder& CharacterBuilder::SetHeight(const float_t height)
{
    m_CharacterHeight = height;
    return *this;
}

CharacterBuilder& CharacterBuilder::SetRadius(const float_t radius)
{
    m_CharacterRadius = radius;
    return *this;
}

Character* CharacterBuilder::BuildAndAdd() const
{
    PhysicsSystem* system = &PhysicsEngine::Instance().GetSystem();
    const ShapeFactory shapeFactory{};

    CharacterSettings settings;
    settings.mShape = shapeFactory.CreateCapsuleShape(m_CharacterHeight, m_CharacterRadius);
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

std::unique_ptr<CharacterVirtual> CharacterBuilder::BuildAndAddVirtual() const
{
    PhysicsSystem* system = &PhysicsEngine::Instance().GetSystem();
    const ShapeFactory shapeFactory{};
    const Vec3 position = JoltUtils::GlmToJoltVec3(m_Position);

    CharacterVirtualSettings settings;
    settings.mMaxSlopeAngle = DegreesToRadians(m_MaxSlopeAngleInDegrees);
    settings.mShape = shapeFactory.CreateCapsuleShape(m_CharacterHeight, m_CharacterRadius);
    settings.mBackFaceMode = EBackFaceMode::CollideWithBackFaces;
    settings.mMaxStrength = 100.0f;
    settings.mCharacterPadding = 0.02f;
    settings.mPenetrationRecoverySpeed = 1.0f;
    settings.mPredictiveContactDistance = 0.1f;
    settings.mSupportingVolume = Plane(Vec3::sAxisY(), -(m_CharacterHeight / 2 + m_CharacterRadius));
    settings.mEnhancedInternalEdgeRemoval = true;

    return std::make_unique<CharacterVirtual>(&settings, position, Quat::sIdentity(), system);
}

}

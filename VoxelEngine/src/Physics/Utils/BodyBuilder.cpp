//
// Created by RadU on 14-Jun-25.
//

#include "BodyBuilder.hpp"

#include "../PhysicsEngineLayers.hpp"

using namespace JPH;

namespace VoxelEngine
{

BodyBuilder::BodyBuilder() = default;

BodyBuilder::~BodyBuilder() = default;

BodyBuilder& BodyBuilder::SetShape(const ShapeRefC& shape)
{
    m_Shape = shape;
    return *this;
}

BodyBuilder& BodyBuilder::SetMotionType(const EMotionType& motionType)
{
    m_MotionType = motionType;
    return *this;
}

BodyBuilder& BodyBuilder::SetMotionQuality(const EMotionQuality& motionQuality)
{
    m_MotionQuality = motionQuality;
    return *this;
}

BodyBuilder& BodyBuilder::SetActivation(const EActivation& activation)
{
    m_Activation = activation;
    return *this;
}

BodyBuilder& BodyBuilder::SetAllowedMovement(const EAllowedDOFs& allowedMovement)
{
    m_AllowedMovement = allowedMovement;
    return *this;
}

BodyBuilder& BodyBuilder::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    return *this;
}

BodyBuilder& BodyBuilder::SetAllowSleeping(const bool& allowSleeping)
{
    m_AllowSleeping = allowSleeping;
    return *this;
}

BodyID BodyBuilder::BuildAndAdd()
{
    const ObjectLayer layer = m_MotionType == EMotionType::Static ? Layers::NON_MOVING : Layers::MOVING;
    const auto jphPosition = Vec3(m_Position.x, m_Position.y, m_Position.z);
    BodyCreationSettings bodySettings(m_Shape, jphPosition, Quat::sIdentity(), m_MotionType, layer);
    bodySettings.mAllowedDOFs = m_AllowedMovement;
    bodySettings.mAllowSleeping = m_AllowSleeping;
    return PhysicsEngine::Instance().GetSystem().GetBodyInterface().CreateAndAddBody(bodySettings, m_Activation);
}

}

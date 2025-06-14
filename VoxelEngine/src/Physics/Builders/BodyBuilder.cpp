//
// Created by RadU on 14-Jun-25.
//

#include "BodyBuilder.hpp"

using namespace JPH;

namespace VoxelEngine
{

BodyBuilder::BodyBuilder()
{
}

BodyBuilder& BodyBuilder::SetShape(const ShapeRefC& shape)
{
    m_Shape = shape;
    return *this;
}

BodyBuilder& BodyBuilder::SetMotionType(const EMotionType& motion)
{
    m_MotionType = motion;
    return *this;
}

BodyBuilder& BodyBuilder::SetActivation(const EActivation& activation)
{
    m_Activation = activation;
    return *this;
}

BodyBuilder& BodyBuilder::SetConstraints(const EAllowedDOFs& constraints)
{
    m_Constraints = constraints;
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
    bodySettings.mAllowedDOFs = m_Constraints;
    bodySettings.mAllowSleeping = m_AllowSleeping;
    return PhysicsEngine::Instance().GetSystem().GetBodyInterface().CreateAndAddBody(bodySettings, m_Activation);
}

}

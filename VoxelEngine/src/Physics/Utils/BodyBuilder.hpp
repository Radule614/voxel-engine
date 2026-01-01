//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"

namespace VoxelEngine
{

class BodyBuilder
{
public:
    BodyBuilder();
    ~BodyBuilder();

    BodyBuilder& SetShape(const JPH::ShapeRefC& shape);
    BodyBuilder& SetMotionType(const JPH::EMotionType& motionType);
    BodyBuilder& SetMotionQuality(const JPH::EMotionQuality& motionQuality);
    BodyBuilder& SetActivation(const JPH::EActivation& activation);
    BodyBuilder& SetAllowedMovement(const JPH::EAllowedDOFs& allowedMovement);
    BodyBuilder& SetPosition(const glm::vec3& position);
    BodyBuilder& SetAllowSleeping(const bool& allowSleeping);

    JPH::BodyID BuildAndAdd();

private:
    JPH::ShapeRefC m_Shape = nullptr;
    JPH::EMotionType m_MotionType = JPH::EMotionType::Static;
    JPH::EMotionQuality m_MotionQuality = JPH::EMotionQuality::Discrete;
    JPH::EActivation m_Activation = JPH::EActivation::DontActivate;
    JPH::EAllowedDOFs m_AllowedMovement = JPH::EAllowedDOFs::All;
    bool m_AllowSleeping = true;
    glm::vec3 m_Position = glm::vec3(0, 0, 0);
};

}

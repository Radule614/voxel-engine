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

    BodyBuilder& SetShape(const JPH::ShapeRefC& shape);
    BodyBuilder& SetMotionType(const JPH::EMotionType& motion);
    BodyBuilder& SetActivation(const JPH::EActivation& activation);
    BodyBuilder& SetConstraints(const JPH::EAllowedDOFs& constraints);
    BodyBuilder& SetPosition(const glm::vec3& position);
    BodyBuilder& SetAllowSleeping(const bool& allowSleeping);

    JPH::BodyID BuildAndAdd();

private:
    JPH::ShapeRefC m_Shape = nullptr;
    JPH::EMotionType m_MotionType = JPH::EMotionType::Static;
    JPH::EActivation m_Activation = JPH::EActivation::DontActivate;
    JPH::EAllowedDOFs m_Constraints = JPH::EAllowedDOFs::All;
    bool m_AllowSleeping = true;
    glm::vec3 m_Position = glm::vec3(0, 0, 0);
};

}

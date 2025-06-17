//
// Created by RadU on 15-Jun-25.
//

#pragma once

#include "../PhysicsEngineLayers.hpp"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

class PlayerCharacterManager
{
public:
    PlayerCharacterManager();
    ~PlayerCharacterManager();

    void UpdateCharacterVirtual(JPH::CharacterVirtual& character, float_t deltaTime, JPH::Vec3 inGravity) const;

private:
    std::unique_ptr<JPH::TempAllocator> m_TempAllocator;
    std::unique_ptr<JPH::BroadPhaseLayerFilter> m_BroadPhaseLayerFilter;
    std::unique_ptr<JPH::ObjectLayerFilter> m_ObjectLayerFilter;
    std::unique_ptr<JPH::BodyFilter> m_BodyFilter;
    std::unique_ptr<JPH::ShapeFilter> m_ShapeFilter;
};

}

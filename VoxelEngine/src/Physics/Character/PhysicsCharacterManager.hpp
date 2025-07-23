//
// Created by RadU on 15-Jun-25.
//

#pragma once

#include "../PhysicsEngineLayers.hpp"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"

namespace VoxelEngine
{

class PhysicsEngine;

class PhysicsCharacterManager
{
public:
    PhysicsCharacterManager(const PhysicsEngine& physicsEngine);
    ~PhysicsCharacterManager();

    void UpdateCharacterVirtual(JPH::CharacterVirtual& character, float_t deltaTime, float_t gravityStrength = 1.0f) const;

private:
    const PhysicsEngine& m_PhysicsEngine;

    std::unique_ptr<JPH::TempAllocator> m_TempAllocator;
    std::unique_ptr<JPH::BroadPhaseLayerFilter> m_BroadPhaseLayerFilter;
    std::unique_ptr<JPH::ObjectLayerFilter> m_ObjectLayerFilter;
    std::unique_ptr<JPH::BodyFilter> m_BodyFilter;
    std::unique_ptr<JPH::ShapeFilter> m_ShapeFilter;
};

}

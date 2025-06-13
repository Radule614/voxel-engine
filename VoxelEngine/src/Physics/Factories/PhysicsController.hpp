//
// Created by RadU on 13-Jun-25.
//

#pragma once

#include "PhysicsFactory.hpp"
#include "../PhysicsEngine.hpp"

namespace VoxelEngine
{

class PhysicsController
{
public:
    PhysicsController();
    ~PhysicsController();

private:
    JPH::PhysicsSystem& m_PhysicsSystem;
    PhysicsFactory m_PhysicsFactory;
};

}

#pragma once

#include <GLCore.hpp>

#include <btBulletDynamicsCommon.h>
#include "Terrain/VoxelLayer.hpp"

using namespace GLCore;

class VoxelEngine : public Application
{
public:
    VoxelEngine() : Application("Voxel Engine")
    {
        btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
        btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();


        PushLayer(new Terrain::VoxelLayer());
    }
};

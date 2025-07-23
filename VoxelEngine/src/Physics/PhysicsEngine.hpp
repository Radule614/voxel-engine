#pragma once

#include "Jolt/Jolt.h"
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include "GLCore.hpp"
#include "Character/PhysicsCharacterManager.hpp"

namespace VoxelEngine
{

class PhysicsEngine
{
public:
    static void Init();
    static void Shutdown();
    static PhysicsEngine& Instance();

    void OnUpdate(GLCore::Timestep ts);
    JPH::PhysicsSystem& GetSystem() const;
    PhysicsCharacterManager& GetPlayerCharacterManager() const;

private:
    PhysicsEngine();
    ~PhysicsEngine();

private:
    double_t m_AccumulatedTime;
    double_t m_FixedTimestep;
    std::unique_ptr<JPH::PhysicsSystem> m_PhysicsSystem;
    std::unique_ptr<JPH::TempAllocator> m_TempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_JobSystem;

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    std::unique_ptr<JPH::BroadPhaseLayerInterface> m_BroadPhaseLayerInterface;
    // Create class that filters object vs broadphase layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilter> m_ObjectVsBroadPhaseLayerFilter;
    // Create class that filters object vs object layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    std::unique_ptr<JPH::ObjectLayerPairFilter> m_ObjectVsObjectLayerFilter;
    // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
    // Note that this is called from a job so whatever you do here needs to be thread safe.
    // Registering one is entirely optional.
    std::unique_ptr<JPH::ContactListener> m_ContactListener;
    // A body activation listener gets notified when bodies activate and go to sleep
    // Note that this is called from a job so whatever you do here needs to be thread safe.
    // Registering one is entirely optional.
    std::unique_ptr<JPH::BodyActivationListener> m_BodyActivationListener;

    std::unique_ptr<PhysicsCharacterManager> m_PlayerCharacterManager;
};

inline PhysicsEngine* g_PhysicsEngine = nullptr;

}

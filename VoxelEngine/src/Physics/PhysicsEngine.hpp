#pragma once

#include "Jolt/Jolt.h"
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include "GLCore.hpp"

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
inline constexpr JPH::ObjectLayer NON_MOVING = 0;
inline constexpr JPH::ObjectLayer MOVING = 1;
inline constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
inline constexpr JPH::BroadPhaseLayer NON_MOVING(0);
inline constexpr JPH::BroadPhaseLayer MOVING(1);
inline constexpr uint32_t NUM_LAYERS(2);
};

namespace VoxelEngine
{

class PhysicsEngine
{
public:
	static void Init();
	static void Shutdown();
	static PhysicsEngine& Instance();

	void OnUpdate(GLCore::Timestep ts);
	JPH::PhysicsSystem& GetSystem();

private:
	PhysicsEngine();
	~PhysicsEngine();

private:
	std::unique_ptr<JPH::PhysicsSystem> m_PhysicsSystem;
	std::unique_ptr<JPH::TempAllocatorImpl> m_TempAllocator;
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

};

inline PhysicsEngine* g_PhysicsEngine = nullptr;

}

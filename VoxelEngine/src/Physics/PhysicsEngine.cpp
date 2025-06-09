#include "PhysicsEngine.hpp"
#include <GLCoreUtils.hpp>

using namespace JPH;
using namespace JPH::literals;

static void TraceImpl(const char* inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    LOG_WARN(buffer);
}

#ifdef JPH_ENABLE_ASSERTS

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine)
{
	LOG_WARN("{0}:{1}:({2})", inFile, inLine, inMessage != nullptr ? inMessage : "");
	return true;
};

#endif // JPH_ENABLE_ASSERTS

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

// An example contact listener
class MyContactListener : public ContactListener
{
public:
    // See: ContactListener
    virtual ValidateResult OnContactValidate(const Body& inBody1,
                                             const Body& inBody2,
                                             RVec3Arg inBaseOffset,
                                             const CollideShapeResult& inCollisionResult) override
    {
        LOG_INFO("Contact validate callback");
        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const Body& inBody1,
                                const Body& inBody2,
                                const ContactManifold& inManifold,
                                ContactSettings& ioSettings) override
    {
        LOG_INFO("A contact was added");
    }

    virtual void OnContactPersisted(const Body& inBody1,
                                    const Body& inBody2,
                                    const ContactManifold& inManifold,
                                    ContactSettings& ioSettings) override
    {
        LOG_INFO("A contact was persisted");
    }

    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
    {
        LOG_INFO("A contact was removed");
    }
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
    virtual void OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
    {
        LOG_INFO("A body got activated");
    }

    virtual void OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
    {
        LOG_INFO("A body went to sleep");
    }
};

namespace VoxelEngine
{

void PhysicsEngine::Init()
{
    g_PhysicsEngine = new PhysicsEngine();
}

void PhysicsEngine::Shutdown()
{
    delete g_PhysicsEngine;
    g_PhysicsEngine = nullptr;
}

PhysicsEngine::PhysicsEngine() : m_AccumulatedTime(0.0),
                                 m_FixedTimestep(1.0 / 60.0),
                                 m_BroadPhaseLayerInterface(std::make_unique<BPLayerInterfaceImpl>()),
                                 m_ObjectVsBroadPhaseLayerFilter(std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>()),
                                 m_ObjectVsObjectLayerFilter(std::make_unique<ObjectLayerPairFilterImpl>()),
                                 m_ContactListener(std::make_unique<MyContactListener>()),
                                 m_BodyActivationListener(std::make_unique<MyBodyActivationListener>())
{
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
    // This needs to be done before any other Jolt function is called.
    RegisterDefaultAllocator();

    // Install trace and assert callbacks
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
    // It is not directly used in this example but still required.
    Factory::sInstance = new Factory();

    // Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
    // If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
    // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
    RegisterTypes();

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    m_TempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads. Typically
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    m_JobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs,
                                                        cMaxPhysicsBarriers,
                                                        thread::hardware_concurrency() - 1);

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const uint cMaxBodies = 65536;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
    const uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
    // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const uint cMaxBodyPairs = 65536;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
    // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
    const uint cMaxContactConstraints = 10240;

    // Now we can create the actual physics system.
    m_PhysicsSystem = std::make_unique<PhysicsSystem>();
    m_PhysicsSystem->Init(cMaxBodies,
                          cNumBodyMutexes,
                          cMaxBodyPairs,
                          cMaxContactConstraints,
                          *m_BroadPhaseLayerInterface.get(),
                          *m_ObjectVsBroadPhaseLayerFilter.get(),
                          *m_ObjectVsObjectLayerFilter.get());
    //m_PhysicsSystem->SetBodyActivationListener(m_BodyActivationListener.get());
    //m_PhysicsSystem->SetContactListener(m_ContactListener.get());
}

PhysicsEngine::~PhysicsEngine()
{
    // Unregisters all types with the factory and cleans up the default material
    UnregisterTypes();

    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

void PhysicsEngine::OnUpdate(GLCore::Timestep ts)
{
    m_AccumulatedTime += ts;
    const int cCollisionSteps = 1;
    while (m_AccumulatedTime >= m_FixedTimestep)
    {
        m_PhysicsSystem->Update(1.0f / 60.0f, 1, m_TempAllocator.get(), m_JobSystem.get());
        m_AccumulatedTime -= m_FixedTimestep;
    }
}

JPH::PhysicsSystem& PhysicsEngine::GetSystem()
{
    return *m_PhysicsSystem.get();
}

PhysicsEngine& PhysicsEngine::Instance()
{
    return *g_PhysicsEngine;
}

}

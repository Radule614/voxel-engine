#include "ECS.hpp"

namespace VoxelEngine
{

void EntityComponentSystem::Init()
{
    g_EntityComponentSystem = new EntityComponentSystem();
}

void EntityComponentSystem::Shutdown()
{
    delete g_EntityComponentSystem;
    g_EntityComponentSystem = nullptr;
}

EntityComponentSystem& EntityComponentSystem::Instance()
{
    return *g_EntityComponentSystem;
}

bool EntityComponentSystem::HasShutdown()
{
    return g_EntityComponentSystem == nullptr;
}

entt::registry& EntityComponentSystem::GetEntityRegistry() const
{
    return *m_EntityRegistry;
}

entt::entity EntityComponentSystem::SafeCreateEntity()
{
    std::lock_guard lock(m_Mutex);
    
    return GetEntityRegistry().create();
}

EntityComponentSystem::EntityComponentSystem() : m_EntityRegistry(std::make_unique<entt::registry>())
{
}

}

#include "ECS.hpp"

namespace VoxelEngine
{

EntityComponentSystem::EntityComponentSystem() : m_EntityRegistry(std::make_unique<entt::registry>())
{
}

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

entt::registry& EntityComponentSystem::GetEntityRegistry() const {
	return *m_EntityRegistry;
}

}
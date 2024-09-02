#pragma once

#include <entt.hpp>

#include "Components/TransformComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/ColliderComponent.hpp"

namespace VoxelEngine
{

class EntityComponentSystem
{
public:
	static void Init();
	static void Shutdown();
	static EntityComponentSystem& Instance();

	entt::registry& GetEntityRegistry();

private:
	EntityComponentSystem();

private:
	std::unique_ptr<entt::registry> m_EntityRegistry;
};

inline EntityComponentSystem* g_EntityComponentSystem = nullptr;

}
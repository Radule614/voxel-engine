
#include "Ecs.hpp"

#include <queue>

#include "Components/ParentComponent.hpp"

namespace VoxelEngine
{

EntityComponentSystem::EntityComponentSystem() : m_EntityRegistry(std::make_unique<entt::registry>())
{
}

void EntityComponentSystem::Init() { g_EntityComponentSystem = new EntityComponentSystem(); }

void EntityComponentSystem::Shutdown()
{
    delete g_EntityComponentSystem;
    g_EntityComponentSystem = nullptr;
}

EntityComponentSystem& EntityComponentSystem::Instance() { return *g_EntityComponentSystem; }

bool EntityComponentSystem::HasShutdown() { return g_EntityComponentSystem == nullptr; }

entt::registry& EntityComponentSystem::GetEntityRegistry() const { return *m_EntityRegistry; }

entt::entity EntityComponentSystem::SafeCreateEntity()
{
    std::lock_guard lock(m_Mutex);

    return GetEntityRegistry().create();
}

void EntityComponentSystem::DestroyEntityRecursive(const entt::entity entity) const
{
    entt::registry& registry = GetEntityRegistry();

    const ParentComponent* parent = registry.try_get<ParentComponent>(entity);
    if (parent != nullptr)
        parent->RemoveChild(entity);

    std::queue<entt::entity> queue;
    queue.push(entity);

    while (!queue.empty())
    {
        const entt::entity current = queue.front();
        queue.pop();

        const ChildrenComponent* children = registry.try_get<ChildrenComponent>(current);
        if (children != nullptr)
        {
            for (auto child: children->Entities)
                queue.push(child);
        }

        registry.destroy(current);
    }
}

}

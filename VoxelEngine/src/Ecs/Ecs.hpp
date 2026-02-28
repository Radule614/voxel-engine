#pragma once

#include <entt.hpp>
#include <mutex>

#include "Components/AnimationComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace VoxelEngine
{

using ComponentsWithGui = std::tuple<
    TransformComponent,
    MeshComponent,
    ColliderComponent,
    AnimationComponent,
    SkinComponent
>;

class EntityComponentSystem
{
public:
    static void Init();
    static void Shutdown();
    static EntityComponentSystem& Instance();
    static bool HasShutdown();

    entt::registry& GetEntityRegistry() const;
    entt::entity SafeCreateEntity();

    void DestroyEntityRecursive(entt::entity entity) const;

private:
    EntityComponentSystem();

private:
    std::unique_ptr<entt::registry> m_EntityRegistry;
    std::mutex m_Mutex;
};

inline EntityComponentSystem* g_EntityComponentSystem = nullptr;

}

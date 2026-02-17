#pragma once

#include <entt.hpp>
#include <mutex>

#include "Components/CameraComponent.hpp"
#include "Components/CharacterComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TerrainMeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace VoxelEngine
{

using ComponentsWithGui = std::tuple<
    TransformComponent,
    MeshComponent,
    ColliderComponent
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

private:
    EntityComponentSystem();

private:
    std::unique_ptr<entt::registry> m_EntityRegistry;
    std::mutex m_Mutex;
};

inline EntityComponentSystem* g_EntityComponentSystem = nullptr;

}

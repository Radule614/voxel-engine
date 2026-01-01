#pragma once

#include <entt.hpp>
#include <mutex>

namespace VoxelEngine
{

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

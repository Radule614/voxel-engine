#include "ExpeditionLayer.hpp"

#include <cstdlib>
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>

#include "Enemy/EnemyComponent.hpp"
#include "Spoils/SpoilsComponent.hpp"
#include "Enemy/EnemyScript.hpp"
#include "Enemy/HealthBarScript.hpp"
#include "Health/HealthComponent.hpp"
#include "Player/PlayerScript.hpp"
#include "Assets/Material.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/EcsUtils.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/ScriptComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Physics/Character/CharacterBuilder.hpp"
#include "Physics/Character/CharacterController.hpp"
#include "Config.hpp"
#include "Terrain/Voxel/VoxelConstants.hpp"
#include "Terrain/World/World.hpp"

namespace Expedition
{

using namespace VoxelEngine;

namespace
{
    constexpr int   MaxEnemies     = 10;
    constexpr float SpawnRadius    = 35.0f;
    constexpr float LeashDistance  = 55.0f;
    constexpr float MinRespawnDist = 20.0f;
    constexpr float MaxRespawnDist = 35.0f;
}

ExpeditionLayer::ExpeditionLayer(EngineState& state, World& world)
    : GLCore::Layer("ExpeditionLayer"), m_State(state), m_World(world)
{
}

void ExpeditionLayer::OnAttach()
{
    SpawnPlayer();

    constexpr int   enemyCount = 3;
    constexpr float y          = 120.0f;
    for (int i = 0; i < enemyCount; ++i)
    {
        const float angle = glm::two_pi<float>() * static_cast<float>(i) / static_cast<float>(enemyCount);
        SpawnEnemy({ glm::cos(angle) * SpawnRadius, y, glm::sin(angle) * SpawnRadius });
    }
}

void ExpeditionLayer::SpawnPlayer()
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto player = registry.create();

    registry.emplace<TransformComponent>(player);
    registry.emplace<MetadataComponent>(player, "Player");

    const auto& cam = m_State.CameraController;

    auto character = CharacterBuilder()
        .SetPosition(cam->GetCamera().GetPosition())
        .BuildAndAddVirtual();

    auto controller = std::make_unique<CharacterController>(std::move(character));
    controller->m_CharacterSpeed = 15.0f;
    controller->m_JumpSpeed = 12.0f;
    controller->m_GravityStrength = 3.0f;
    controller->m_InertiaEnabled = true;

    registry.emplace<CharacterComponent>(player, std::move(controller));
    registry.emplace<CameraComponent>(player, cam);
    registry.emplace<HealthComponent>(player);
    registry.emplace<SpoilsComponent>(player);

    auto& scriptComponent = registry.emplace<ScriptComponent>(player);
    scriptComponent.Scripts.emplace_back(std::make_unique<PlayerScript>());
}

void ExpeditionLayer::SpawnEnemy(const glm::vec3 position)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    // Visual child: Mutant model
    static Model* mutantModel = AssetManager::Instance().LoadModel("assets/models/Mutant.glb");
    const auto modelEntity = CreateEntityFromModel(*mutantModel);
    auto& modelTransform = registry.get<TransformComponent>(modelEntity);
    modelTransform.LocalPosition = glm::vec3(0.0f, -1.5f, 0.0f);
    modelTransform.LocalScale    = glm::vec3(2.0f);
    modelTransform.IsDirty = true;

    // Physics parent — capsule total height = height + 2*radius = 2.0 + 1.5 = 3.5
    JPH::Character* raw = CharacterBuilder()
        .SetHeight(2.0f)
        .SetRadius(0.75f)
        .SetPosition(position)
        .BuildAndAdd();

    const ColliderComponent collider(raw->GetBodyID(),
                                     raw->GetShape()->GetType(),
                                     raw->GetShape()->GetSubType());

    const auto parent = registry.create();
    registry.emplace<TransformComponent>(parent);
    registry.emplace<MetadataComponent>(parent, "Enemy");
    registry.emplace<ColliderComponent>(parent, collider);
    registry.emplace<EnemyComponent>(parent, raw);
    registry.emplace<HealthComponent>(parent);

    auto& scriptComponent = registry.emplace<ScriptComponent>(parent);
    scriptComponent.Scripts.emplace_back(std::make_unique<EnemyScript>(m_World));

    AddChildToEntity(parent, modelEntity);
    SpawnHealthBar(parent);
}

void ExpeditionLayer::SpawnHealthBar(const entt::entity enemyParent)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    constexpr float barWidth  = 1.2f;
    constexpr float barHeight = 0.12f;
    constexpr float yOffset   = 3.6f;

    // Root entity — no mesh, just holds the billboard rotation and Y offset
    const auto root = registry.create();
    auto& rootT = registry.emplace<TransformComponent>(root);
    rootT.LocalPosition = glm::vec3(0.0f, yOffset, 0.0f);
    rootT.IsDirty = true;
    AddChildToEntity(enemyParent, root);

    // Background quad (red)
    auto bgMesh = MeshComponent::CreateQuadMesh();
    auto& bgPrims = const_cast<std::vector<RenderPrimitive>&>(bgMesh.Primitives);
    bgPrims[0].Material.AlbedoFactor    = glm::vec4(0.85f, 0.1f, 0.1f, 1.0f);
    bgPrims[0].Material.MetallicFactor  = 0.0f;
    bgPrims[0].Material.RoughnessFactor = 1.0f;

    const auto bg = registry.create();
    auto& bgT = registry.emplace<TransformComponent>(bg);
    bgT.LocalScale = glm::vec3(barWidth, barHeight, 1.0f);
    bgT.IsDirty = true;
    registry.emplace<MeshComponent>(bg, bgMesh.Name, bgMesh.Primitives);
    AddChildToEntity(root, bg);

    // Fill quad (green, child of bg so z-offset is in bg's local space)
    auto fillMesh = MeshComponent::CreateQuadMesh();
    auto& fillPrims = const_cast<std::vector<RenderPrimitive>&>(fillMesh.Primitives);
    fillPrims[0].Material.AlbedoFactor    = glm::vec4(0.1f, 0.85f, 0.1f, 1.0f);
    fillPrims[0].Material.MetallicFactor  = 0.0f;
    fillPrims[0].Material.RoughnessFactor = 1.0f;

    const auto fill = registry.create();
    auto& fillT = registry.emplace<TransformComponent>(fill);
    // Scale is in bg's local space (bg is already {barWidth, barHeight}),
    // so {1, 0.8, 1} means "same width as bg, 80% height"
    fillT.LocalScale    = glm::vec3(1.0f, 0.8f, 1.0f);
    fillT.LocalPosition = glm::vec3(0.0f, 0.0f, 0.001f);
    fillT.IsDirty = true;
    registry.emplace<MeshComponent>(fill, fillMesh.Name, fillMesh.Primitives);
    AddChildToEntity(bg, fill);

    // Attach script to enemy parent
    auto& sc = registry.get<ScriptComponent>(enemyParent);
    sc.Scripts.emplace_back(std::make_unique<HealthBarScript>(root, fill, barWidth));
}

void ExpeditionLayer::OnUpdate(const GLCore::Timestep ts)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    // Find player position
    glm::vec3 playerPos = glm::vec3(0.0f);
    bool      hasPlayer = false;
    for (auto [e, tc, cc] : registry.view<TransformComponent, CharacterComponent>().each())
    {
        playerPos = tc.WorldPosition;
        hasPlayer = true;
        break;
    }

    // Collect enemies whose death animation has finished
    std::vector<entt::entity> dead;
    for (auto [entity, enemy] : registry.view<EnemyComponent>().each())
    {
        if (enemy.ReadyToRemove)
            dead.push_back(entity);
    }

    // Award spoils for kills
    if (!dead.empty())
    {
        for (auto [e, spoils, character] : registry.view<SpoilsComponent, CharacterComponent>().each())
        {
            spoils.Add(50 * static_cast<int>(dead.size()));
            break;
        }
    }

    // Collect enemies that are too far (leash) — skip dying enemies
    if (hasPlayer)
    {
        for (auto [entity, tc, enemy] : registry.view<TransformComponent, EnemyComponent>().each())
        {
            if (enemy.IsDying)
                continue;
            const float dx   = tc.WorldPosition.x - playerPos.x;
            const float dz   = tc.WorldPosition.z - playerPos.z;
            const float dist = glm::sqrt(dx * dx + dz * dz);
            if (dist > LeashDistance)
            {
                if (std::find(dead.begin(), dead.end(), entity) == dead.end())
                    dead.push_back(entity);
            }
        }
    }

    // How many to respawn: clamp to MaxEnemies
    const int liveCount  = static_cast<int>(registry.view<EnemyComponent>().size());
    const int toRemove   = static_cast<int>(dead.size());
    const int afterKills = liveCount - toRemove;
    const int toRespawn  = std::min(toRemove, MaxEnemies - afterKills);

    for (const auto entity : dead)
    {
        auto& enemy = registry.get<EnemyComponent>(entity);
        if (enemy.Character)  // leash evictions still have physics; ReadyToRemove ones don't
            enemy.Character->RemoveFromPhysicsSystem();
        EntityComponentSystem::Instance().DestroyEntityRecursive(entity);
    }

    if (hasPlayer)
    {
        for (int i = 0; i < toRespawn; ++i)
            SpawnEnemy(RandomSpawnNearPlayer(MinRespawnDist, MaxRespawnDist));
    }
}

glm::vec3 ExpeditionLayer::RandomSpawnNearPlayer(const float minR, const float maxR) const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    glm::vec3 pPos = glm::vec3(0.0f, 120.0f, 0.0f);
    for (auto [e, tc, cc] : registry.view<TransformComponent, CharacterComponent>().each())
    {
        pPos = tc.WorldPosition;
        break;
    }
    const float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * glm::two_pi<float>();
    const float dist  = minR + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxR - minR);

    const float spawnX = pPos.x + glm::cos(angle) * dist;
    const float spawnZ = pPos.z + glm::sin(angle) * dist;

    // Find the terrain surface by scanning from the top of the chunk downward
    const int ix = static_cast<int>(std::floor(spawnX));
    const int iz = static_cast<int>(std::floor(spawnZ));
    auto [chunkPos, voxelPos] = World::GlobalToWorldSpace(glm::i32vec3(ix, 0, iz));
    const auto& chunkMap = m_World.GetChunkMap();
    auto it = chunkMap.find(chunkPos);

    float spawnY = pPos.y; // fallback to player Y
    if (it != chunkMap.end())
    {
        for (int y = CHUNK_HEIGHT - 1; y >= 0; --y)
        {
            auto [cp, vp] = World::GlobalToWorldSpace(glm::i32vec3(ix, y, iz));
            const auto& voxel = it->second->GetVoxelFromGrid(vp);
            if (voxel.GetVoxelType() != AIR)
            {
                spawnY = static_cast<float>(y + 2);
                break;
            }
        }
    }

    return { spawnX, spawnY, spawnZ };
}

void ExpeditionLayer::OnImGuiRender()
{
    // Draw directly onto the Viewport window's draw list so the HUD
    // stays inside the game panel regardless of editor layout.
    ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
    if (!viewport)
        return;

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    float current = 0.0f;
    float max     = 100.0f;
    for (auto [entity, health, character] : registry.view<HealthComponent, CharacterComponent>().each())
    {
        current = health.CurrentHealth;
        max     = health.MaxHealth;
        break;
    }

    const float pct = max > 0.0f ? glm::clamp(current / max, 0.0f, 1.0f) : 0.0f;

    constexpr float barW = 300.0f;
    constexpr float barH = 20.0f;
    constexpr float padX = 20.0f;
    constexpr float padY = 20.0f;

    const ImVec2 vMin = viewport->InnerRect.Min;
    const ImVec2 vMax = viewport->InnerRect.Max;

    const ImVec2 pos = {
        vMin.x + padX,
        vMax.y - padY - barH
    };

    ImDrawList* draw = viewport->DrawList;

    // Background (dark red)
    draw->AddRectFilled(pos, { pos.x + barW, pos.y + barH },
        IM_COL32(120, 20, 20, 220), 4.0f);

    // Fill (green → yellow → red)
    if (pct > 0.0f)
    {
        const ImVec4 col = pct > 0.5f
            ? ImVec4(0.1f + (1.0f - pct) * 1.8f, 0.85f, 0.1f, 1.0f)
            : ImVec4(0.85f, pct * 1.7f, 0.1f, 1.0f);
        draw->AddRectFilled(pos, { pos.x + barW * pct, pos.y + barH },
            ImGui::ColorConvertFloat4ToU32(col), 4.0f);
    }

    // Label
    char buf[32];
    snprintf(buf, sizeof(buf), "HP  %.0f / %.0f", current, max);
    draw->AddText({ pos.x + 6.0f, pos.y + 3.0f }, IM_COL32(255, 255, 255, 220), buf);

    // Spoils
    int spoilsAmt = 0;
    for (auto [e, spoils, character] : registry.view<SpoilsComponent, CharacterComponent>().each())
    {
        spoilsAmt = spoils.Amount;
        break;
    }
    char spoilsBuf[32];
    snprintf(spoilsBuf, sizeof(spoilsBuf), "Spoils: %d", spoilsAmt);
    draw->AddText({ pos.x, pos.y - 20.0f }, IM_COL32(255, 215, 0, 220), spoilsBuf);
}

}

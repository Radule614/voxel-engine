#include "WorldLayer.hpp"

#include "Enemy/EnemyComponent.hpp"
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

namespace Expedition
{

using namespace VoxelEngine;

WorldLayer::WorldLayer(EngineState& state)
    : GLCore::Layer("WorldLayer"), m_State(state)
{
}

void WorldLayer::OnAttach()
{
    SpawnPlayer();

    constexpr int   enemyCount = 3;
    constexpr float radius     = 10.0f;
    constexpr float y          = 120.0f;
    for (int i = 0; i < enemyCount; ++i)
    {
        const float angle = glm::two_pi<float>() * static_cast<float>(i) / static_cast<float>(enemyCount);
        SpawnEnemy({ glm::cos(angle) * radius, y, glm::sin(angle) * radius });
    }
}

void WorldLayer::SpawnPlayer()
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

    auto& scriptComponent = registry.emplace<ScriptComponent>(player);
    scriptComponent.Scripts.emplace_back(std::make_unique<PlayerScript>());
}

void WorldLayer::SpawnEnemy(const glm::vec3 position)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    // Visual child: Mutant model
    static Model* mutantModel = AssetManager::Instance().LoadModel("assets/models/Mutant.glb");
    const auto modelEntity = CreateEntityFromModel(*mutantModel);
    auto& modelTransform = registry.get<TransformComponent>(modelEntity);
    modelTransform.LocalPosition = glm::vec3(0.0f, -1.5f, 0.0f);
    modelTransform.IsDirty = true;

    // Physics parent
    JPH::Character* raw = CharacterBuilder()
        .SetHeight(2.0f)
        .SetRadius(0.5f)
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
    scriptComponent.Scripts.emplace_back(std::make_unique<EnemyScript>());

    AddChildToEntity(parent, modelEntity);
    SpawnHealthBar(parent);
}

void WorldLayer::SpawnHealthBar(const entt::entity enemyParent)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    constexpr float barWidth  = 1.2f;
    constexpr float barHeight = 0.12f;
    constexpr float yOffset   = 2.8f;

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

void WorldLayer::OnUpdate(const GLCore::Timestep ts)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    std::vector<entt::entity> dead;
    for (auto [entity, health, enemy] : registry.view<HealthComponent, EnemyComponent>().each())
    {
        if (health.IsDead())
            dead.push_back(entity);
    }

    for (const auto entity : dead)
    {
        auto& enemy = registry.get<EnemyComponent>(entity);
        if (enemy.Character)
            enemy.Character->RemoveFromPhysicsSystem();
        EntityComponentSystem::Instance().DestroyEntityRecursive(entity);
    }
}

}

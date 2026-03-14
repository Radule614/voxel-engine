#include "WorldLayer.hpp"

#include "Enemy/EnemyComponent.hpp"
#include "Enemy/EnemyScript.hpp"
#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
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
    SpawnEnemy({5.0f, 120.0f, 5.0f});
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
}

void WorldLayer::SpawnEnemy(const glm::vec3 position)
{
    static Model* capsuleModel = AssetManager::Instance().LoadModel("assets/models/capsule/Capsule.glb");

    JPH::Character* raw = CharacterBuilder()
        .SetHeight(2.0f)
        .SetRadius(0.5f)
        .SetPosition(position)
        .BuildAndAdd();

    const ColliderComponent collider(raw->GetBodyID(),
                                     raw->GetShape()->GetType(),
                                     raw->GetShape()->GetSubType());

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto enemy = CreateEntityFromModel(*capsuleModel);
    registry.get<MetadataComponent>(enemy).Name = "Enemy";
    registry.emplace<ColliderComponent>(enemy, collider);
    registry.emplace<EnemyComponent>(enemy, raw);

    auto& scriptComponent = registry.emplace<ScriptComponent>(enemy);
    scriptComponent.Scripts.emplace_back(std::make_unique<EnemyScript>());
}

}

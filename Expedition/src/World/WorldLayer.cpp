#include "WorldLayer.hpp"

#include "Enemy/EnemyComponent.hpp"
#include "Enemy/EnemyScript.hpp"
#include "Player/PlayerScript.hpp"
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

    auto& scriptComponent = registry.emplace<ScriptComponent>(parent);
    scriptComponent.Scripts.emplace_back(std::make_unique<EnemyScript>());

    AddChildToEntity(parent, modelEntity);
}

}

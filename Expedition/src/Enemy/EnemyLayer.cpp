#include "EnemyLayer.hpp"
#include "EnemyScript.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/ScriptComponent.hpp"
#include "Physics/Character/CharacterBuilder.hpp"
#include "Physics/Character/CharacterController.hpp"

namespace Expedition
{

using namespace VoxelEngine;

EnemyLayer::EnemyLayer(EngineState& state)
    : GLCore::Layer("EnemyLayer"), m_State(state)
{
}

void EnemyLayer::OnAttach()
{
    static Model* capsuleModel = AssetManager::Instance().LoadModel("assets/models/capsule/Capsule.glb");

    auto character = CharacterBuilder()
        .SetHeight(2.0f)
        .SetRadius(0.5f)
        .SetPosition({5.0f, 120.0f, 5.0f})
        .BuildAndAddVirtual();

    auto controller = std::make_unique<CharacterController>(std::move(character));
    controller->m_CharacterSpeed = 4.0f;
    controller->m_GravityStrength = 3.0f;

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto enemy = CreateEntityFromModel(*capsuleModel);
    registry.get<MetadataComponent>(enemy).Name = "Enemy";
    registry.emplace<CharacterComponent>(enemy, std::move(controller));

    auto& scriptComponent = registry.emplace<ScriptComponent>(enemy);
    scriptComponent.Scripts.emplace_back(std::make_unique<EnemyScript>());
}

}

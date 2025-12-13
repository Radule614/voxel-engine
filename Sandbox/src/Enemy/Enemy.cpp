//
// Created by RadU on 11/1/2025.
//

#include "Enemy.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"

using namespace JPH;
using namespace VoxelEngine;

namespace Sandbox
{

Enemy::Enemy(const glm::vec3 position)
{
    static Model* capsuleModel = AssetManager::Instance().LoadModel("assets/models/capsule/Capsule.glb");

    Character* character = CharacterBuilder()
            .SetHeight(2.0f)
            .SetPosition(position)
            .SetRadius(0.5f)
            .BuildAndAdd();

    m_Character = std::unique_ptr<Character>(character);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    m_Entity = registry.create();
    registry.emplace<MeshComponent>(m_Entity, *capsuleModel);
    registry.emplace<TransformComponent>(m_Entity, TransformComponent{});
    registry.emplace<ColliderComponent>(m_Entity, ColliderComponent(character->GetBodyID()));
}

Enemy::~Enemy() = default;

}

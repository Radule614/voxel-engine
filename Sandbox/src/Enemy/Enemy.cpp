//
// Created by RadU on 11/1/2025.
//

#include "Enemy.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
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

    ColliderComponent collider(character->GetBodyID(),
                               character->GetShape()->GetType(),
                               character->GetShape()->GetSubType());

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    m_Entity = CreateEntityFromModel(*capsuleModel);
    registry.emplace<ColliderComponent>(m_Entity, collider);
    registry.get<MetadataComponent>(m_Entity).Name = "Enemy";
}

Enemy::~Enemy() = default;

}

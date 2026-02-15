//
// Created by RadU on 12/13/2025.
//

#include "SponzaLayer.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace VoxelEngine;

namespace Sandbox
{

SponzaLayer::SponzaLayer(EngineState& state) : m_State(state)
{
}

SponzaLayer::~SponzaLayer() = default;

static entt::entity AnimatedModelEntity;

void SponzaLayer::OnAttach()
{
    static Model* sponzaModel = AssetManager::Instance().LoadModel("assets/models/sponza/Sponza.glb");
    // static Model* model = AssetManager::Instance().LoadModel("assets/models/EnvironmentTest.glb");
    static Model* animatedModel = AssetManager::Instance().LoadModel("assets/models/BoxAnimated.glb");

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const auto sponzaEntity = CreateEntityFromModel(*sponzaModel);
    auto& transform = registry.get<TransformComponent>(sponzaEntity);
    transform.LocalPosition = glm::vec3(0, -1, 0);
    transform.LocalRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    transform.IsDirty = true;

    AnimatedModelEntity = CreateEntityFromModel(*animatedModel);
}

void SponzaLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) { return false; });
}


}

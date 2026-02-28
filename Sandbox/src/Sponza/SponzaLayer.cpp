//
// Created by RadU on 12/13/2025.
//

#include "SponzaLayer.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/LightComponent.hpp"
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

void SponzaLayer::OnAttach()
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    static Model* sponzaModel = AssetManager::Instance().LoadModel("assets/models/sponza/Sponza.glb");
    const auto sponzaEntity = CreateEntityFromModel(*sponzaModel);
    auto& sponzaTransform = registry.get<TransformComponent>(sponzaEntity);
    sponzaTransform.LocalPosition = glm::vec3(0, -1, 0);
    sponzaTransform.LocalRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    sponzaTransform.IsDirty = true;

    // static Model* animatedModel = AssetManager::Instance().LoadModel("assets/models/BoxAnimated.glb");
    // CreateEntityFromModel(*animatedModel);

    // static Model* interpolationTestModel = AssetManager::Instance().LoadModel("assets/models/InterpolationTest.glb");
    // CreateEntityFromModel(*interpolationTestModel);

    // static Model* brainStemModel = AssetManager::Instance().LoadModel("assets/models/BrainStem.glb");
    // const auto brainStemEntity = CreateEntityFromModel(*brainStemModel);
    // auto& brainStemTransform = registry.get<TransformComponent>(brainStemEntity);
    // brainStemTransform.LocalPosition = glm::vec3(0, -1.0f, 0);
    // brainStemTransform.LocalRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // brainStemTransform.IsDirty = true;

    const entt::entity lightEntity = registry.create();
    PointLight pointLight(glm::vec3(0, 0.5, 3), glm::vec3(0, 0.5, 1));
    registry.emplace<LightComponent>(lightEntity, pointLight);

    static Model* paladinModel = AssetManager::Instance().LoadModel("assets/models/Paladin.glb");
    const auto paladinEntity = CreateEntityFromModel(*paladinModel);
    auto& paladinTransform = registry.get<TransformComponent>(paladinEntity);
    paladinTransform.LocalPosition = glm::vec3(0, -1.0f, 0);
    paladinTransform.IsDirty = true;
}

void SponzaLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) { return false; });
}


}

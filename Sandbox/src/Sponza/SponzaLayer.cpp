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

void SponzaLayer::OnAttach()
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    // static Model* sponzaModel = AssetManager::Instance().LoadModel("assets/models/sponza/Sponza.glb");
    // const auto sponzaEntity = CreateEntityFromModel(*sponzaModel);
    // auto& transform = registry.get<TransformComponent>(sponzaEntity);
    // transform.LocalPosition = glm::vec3(0, -1, 0);
    // transform.LocalRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // transform.IsDirty = true;

    // static Model* animatedModel = AssetManager::Instance().LoadModel("assets/models/BoxAnimated.glb");
    // CreateEntityFromModel(*animatedModel);

    static Model* interpolationTestModel = AssetManager::Instance().LoadModel("assets/models/InterpolationTest.glb");
    CreateEntityFromModel(*interpolationTestModel);
}

void SponzaLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) { return false; });
}


}

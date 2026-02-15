//
// Created by RadU on 12/13/2025.
//

#include "SponzaLayer.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"

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
    // static Model* sponzaModel = AssetManager::Instance().LoadModel("assets/models/sponza/Sponza.glb");
    // static Model* model = AssetManager::Instance().LoadModel("assets/models/EnvironmentTest.glb");
    static Model* animatedModel = AssetManager::Instance().LoadModel("assets/models/BoxAnimated.glb");

    const auto& cameraController = m_State.CameraController;
    const auto cameraPosition = cameraController->GetCamera().GetPosition();

    TransformComponent transform{};
    transform.LocalPosition = cameraPosition + glm::vec3(0.0f, 0.0f, -3.0f);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    // const auto sponzaEntity = registry.create();
    // registry.emplace<MeshComponent>(sponzaEntity, *sponzaModel);
    // registry.emplace<TransformComponent>(sponzaEntity, TransformComponent{});
    // registry.emplace<MetadataComponent>(sponzaEntity, "Sponza");

    AnimatedModelEntity = registry.create();
    registry.emplace<MeshComponent>(AnimatedModelEntity, *animatedModel);
    registry.emplace<TransformComponent>(AnimatedModelEntity, transform);
    registry.emplace<MetadataComponent>(AnimatedModelEntity, "BoxAnimated");
}

void SponzaLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) { return false; });
}


}

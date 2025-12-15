//
// Created by RadU on 12/13/2025.
//

#include "SponzaLayer.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/MeshComponent.hpp"
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

void SponzaLayer::OnAttach()
{
    static Model* model = AssetManager::Instance().LoadModel("assets/models/sponza/Sponza.glb");

    const auto& cameraController = m_State.CameraController;
    const auto cameraPosition = cameraController->GetCamera().GetPosition();

    TransformComponent transform{};
    transform.Position = cameraPosition + glm::vec3(0.0f, 0.0f, -10.0f);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto entity = registry.create();

    registry.emplace<MeshComponent>(entity, *model);
    registry.emplace<TransformComponent>(entity, transform);
}

}

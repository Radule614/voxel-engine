#include "PlayerLayer.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/CameraComponent.hpp"
#include "Physics/Character/CharacterBuilder.hpp"
#include "Physics/Character/CharacterController.hpp"

namespace Expedition
{
using namespace VoxelEngine;

PlayerLayer::PlayerLayer(EngineState& state)
    : GLCore::Layer("PlayerLayer"), m_State(state) {}

void PlayerLayer::OnAttach()
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
}

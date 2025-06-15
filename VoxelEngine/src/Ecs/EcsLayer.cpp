#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Physics/Utils/JoltUtils.hpp"
#include "../Renderer/Renderer.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "GLCore/Core/Input.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

namespace VoxelEngine
{

EcsLayer::EcsLayer(EngineState& state) : m_State(state)
{
}

EcsLayer::~EcsLayer() = default;

void EcsLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void EcsLayer::OnDetach()
{
}

void EcsLayer::OnUpdate(const Timestep ts)
{
    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
    BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    PlayerCharacterManager& playerCharacterManager = PhysicsEngine::Instance().GetPlayerCharacterManager();
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto view = registry.view<ColliderComponent, TransformComponent>(); const auto entity: view)
    {
        const auto& collider = view.get<ColliderComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);
        if (!bodyInterface.IsActive(collider.BodyId))
            continue;
        UpdateTransformComponent(transform, collider.BodyId);
        RaiseColliderLocationChangedEvent(transform);
    }

    for (const auto view = registry.view<TransformComponent, PlayerComponent>(); const auto entity: view)
    {
        auto& character = *view.get<PlayerComponent>(entity).Character;
        auto& transform = view.get<TransformComponent>(entity);

        glm::vec3 v = JoltUtils::JoltToGlmVec3(character.GetLinearVelocity());

        if (registry.all_of<CameraComponent>(entity))
        {
            auto& controller = *registry.get<CameraComponent>(entity).CameraController;
            const auto& movement = controller.CalculateMovementVector(ts);

            glm::vec2 xz(0.0f);
            if (movement.x != 0.0f || movement.z != 0.0f)
                xz = 10.0f * glm::normalize(glm::vec2(movement.x, movement.z));
            else if (glm::abs(v.x) > 1.0f || glm::abs(v.z) > 1.0f)
                xz = glm::normalize(glm::vec2(v.x, v.z));

            v.x = xz.x;
            v.z = xz.y;

            if (Input::IsKeyPressed(HZ_KEY_SPACE) && character.GetGroundState() ==
                CharacterVirtual::EGroundState::OnGround)
                v.y = 10.0f;

            v.y = physicsSystem.GetGravity().GetY();

            controller.GetCamera().SetPosition(transform.Position);
        }

        character.SetLinearVelocity(JoltUtils::GlmToJoltVec3(v));
        playerCharacterManager.UpdateCharacterVirtual(character, ts, physicsSystem.GetGravity());
        UpdateTransformComponent(transform, character);
        RaiseColliderLocationChangedEvent(transform);
    }

    Renderer::Instance().RenderScene(m_State.CameraController->GetCamera());
}

void EcsLayer::UpdateTransformComponent(TransformComponent& transform, const BodyID& bodyId)
{
    const BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
    const Vec3 position = bodyInterface.GetCenterOfMassPosition(bodyId);
    Vec3 axis{};
    float_t angle;
    bodyInterface.GetRotation(bodyId).GetAxisAngle(axis, angle);
    UpdateTransformComponent(transform, JoltUtils::JoltToGlmVec3(position), angle, JoltUtils::JoltToGlmVec3(axis));
}

void EcsLayer::UpdateTransformComponent(TransformComponent& transform, const CharacterVirtual& character)
{
    const Vec3 position = character.GetPosition();
    Vec3 axis{};
    float_t angle;
    character.GetRotation().GetAxisAngle(axis, angle);
    UpdateTransformComponent(transform, JoltUtils::JoltToGlmVec3(position), angle, JoltUtils::JoltToGlmVec3(axis));
}

void EcsLayer::UpdateTransformComponent(TransformComponent& transform,
                                        const glm::vec3 position,
                                        const float_t angle,
                                        const glm::vec3 axis)
{
    transform.PreviousPosition = transform.Position;
    transform.Position = position;
    transform.RotationAngle = angle;
    if (axis != glm::vec3(0.0f))
        transform.RotationAxis = axis;
    else
        transform.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
}

void EcsLayer::RaiseColliderLocationChangedEvent(const TransformComponent& transform)
{
    if (static_cast<glm::ivec3>(glm::round(transform.PreviousPosition)) !=
        static_cast<glm::ivec3>(glm::round(transform.Position)))
    {
        ColliderLocationChangedEvent event(transform.Position);
        m_State.Application->RaiseEvent(event);
    }
}

}

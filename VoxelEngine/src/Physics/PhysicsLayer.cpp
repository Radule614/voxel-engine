#include "PhysicsLayer.hpp"

#include "PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Physics/Utils/JoltUtils.hpp"
#include "GLCore/Core/Input.hpp"
#include "Character/CharacterController.hpp"

#include "../Ecs/Components/CameraComponent.hpp"
#include "../Ecs/Components/TransformComponent.hpp"
#include "../Ecs/Components/ColliderComponent.hpp"
#include "../Ecs/Components/CharacterComponent.hpp"
#include "../Ecs/Scene.hpp"
#include "glm/gtx/matrix_decompose.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

namespace VoxelEngine
{

static TransformComponent GetParentTransform(entt::registry& registry, entt::entity entity);

static void UpdateTransformComponent(
    TransformComponent& transform,
    const TransformComponent& parentTransform,
    const BodyID& bodyId);

static void UpdateTransformComponent(
    TransformComponent& transform,
    const TransformComponent& parentTransform,
    const CharacterController& controller);

static void RaiseColliderLocationChangedEvent(const TransformComponent& transform, Application& application);

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
}

void PhysicsLayer::OnAttach()
{
}

void PhysicsLayer::OnUpdate(const Timestep ts)
{
    if (m_State.MenuActive)
        return;

    PhysicsEngine::Instance().OnUpdate(ts);

    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
    BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    PhysicsCharacterManager& physicsCharacterManager = PhysicsEngine::Instance().GetPlayerCharacterManager();
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto view = registry.view<ColliderComponent, TransformComponent>(); const auto entity: view)
    {
        const auto& collider = view.get<ColliderComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        const bool isBodyActive = bodyInterface.IsActive(collider.BodyId);
        const bool isBodyStatic = bodyInterface.GetMotionType(collider.BodyId) == EMotionType::Static;

        if (!isBodyActive || isBodyStatic)
            continue;

        UpdateTransformComponent(transform, GetParentTransform(registry, entity), collider.BodyId);
        RaiseColliderLocationChangedEvent(transform, *m_State.Application);
    }

    for (const auto& view = registry.view<TransformComponent, CharacterComponent>(); const auto entity: view)
    {
        auto& characterController = *view.get<CharacterComponent>(entity).Controller;
        auto& velocity = view.get<CharacterComponent>(entity).Velocity;
        auto& transform = view.get<TransformComponent>(entity);

        auto cameraComponent = registry.try_get<CameraComponent>(entity);
        if (cameraComponent != nullptr)
        {
            auto& cameraController = *cameraComponent->CameraController;
            cameraController.GetCamera().SetPosition(transform.WorldPosition);

            velocity = cameraController.CalculateMovementDirection();
            characterController.HandleInput(JoltUtils::GlmToJoltVec3(velocity), Input::IsKeyPressed(VE_KEY_SPACE), ts);
        }

        physicsCharacterManager.UpdateCharacterVirtual(characterController.GetCharacter(),
                                                       ts,
                                                       characterController.m_GravityStrength);
        UpdateTransformComponent(transform, GetParentTransform(registry, entity), characterController);
        RaiseColliderLocationChangedEvent(transform, *m_State.Application);
    }
}

static TransformComponent GetParentTransform(entt::registry& registry, const entt::entity entity)
{
    const auto parentComponent = registry.try_get<ParentComponent>(entity);

    return parentComponent != nullptr
                                ? registry.get<TransformComponent>(parentComponent->Entity)
                                : TransformComponent{};
}

static void SyncPhysicsWorldTransform(
    TransformComponent& transform,
    const glm::vec3& worldPosition,
    const glm::quat& worldRotation,
    const TransformComponent& parentTransform)
{
    const glm::mat4 physicsWorld =
            glm::translate(glm::mat4(1.0f), worldPosition) *
            glm::mat4_cast(worldRotation);

    const glm::mat4 local = glm::inverse(parentTransform.WorldMatrix) * physicsWorld;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(local, scale, rotation, position, skew, perspective);

    transform.LocalPosition = position;
    transform.LocalRotation = glm::normalize(rotation);
    transform.IsDirty = true;
}

static void UpdateTransformComponent(
    TransformComponent& transform,
    const TransformComponent& parentTransform,
    const BodyID& bodyId)
{
    const BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();

    const glm::vec3 worldPosition = JoltUtils::JoltToGlmVec3(bodyInterface.GetCenterOfMassPosition(bodyId));
    const glm::quat worldRotation = JoltUtils::JoltToGlmQuat(bodyInterface.GetRotation(bodyId));

    SyncPhysicsWorldTransform(transform, worldPosition, worldRotation, parentTransform);
}

static void UpdateTransformComponent(
    TransformComponent& transform,
    const TransformComponent& parentTransform,
    const CharacterController& controller)
{
    const CharacterVirtual& character = controller.GetCharacter();

    const glm::vec3 worldPosition = JoltUtils::JoltToGlmVec3(character.GetPosition());
    const glm::quat worldRotation = JoltUtils::JoltToGlmQuat(character.GetRotation());

    SyncPhysicsWorldTransform(transform, worldPosition, worldRotation, parentTransform);
}

static void RaiseColliderLocationChangedEvent(const TransformComponent& transform, Application& application)
{
    if (static_cast<glm::ivec3>(glm::round(transform.PreviousWorldPosition)) !=
        static_cast<glm::ivec3>(glm::round(transform.WorldPosition)))
    {
        ColliderLocationChangedEvent event(transform.WorldPosition);
        application.RaiseEvent(event);
    }
}

}

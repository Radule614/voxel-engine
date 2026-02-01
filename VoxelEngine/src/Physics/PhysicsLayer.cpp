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

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

namespace VoxelEngine
{

static void UpdateTransformComponent(TransformComponent& transform, const BodyID& bodyId);
static void UpdateTransformComponent(TransformComponent& transform, const CharacterController& controller);
static void UpdateTransformComponent(TransformComponent& transform, glm::vec3 position, float_t angle, glm::vec3 axis);
static void RaiseColliderLocationChangedEvent(const TransformComponent& transform, Application& application);

static void UpdateTransformRecursive(entt::entity entity, const TransformComponent& parentTransform);

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

    const auto rootView = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto rootEntity: rootView)
    {
        UpdateTransformRecursive(rootEntity, TransformComponent{});
    }

    for (const auto view = registry.view<ColliderComponent, TransformComponent>(); const auto entity: view)
    {
        const auto& collider = view.get<ColliderComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        const bool isBodyActive = bodyInterface.IsActive(collider.BodyId);
        const bool isBodyStatic = bodyInterface.GetMotionType(collider.BodyId) == EMotionType::Static;

        if (!isBodyActive || isBodyStatic)
            continue;

        UpdateTransformComponent(transform, collider.BodyId);
        RaiseColliderLocationChangedEvent(transform, *m_State.Application);
    }

    for (const auto& view = registry.view<TransformComponent, CharacterComponent>(); const auto entity: view)
    {
        auto& characterController = *view.get<CharacterComponent>(entity).Controller;
        auto& velocity = view.get<CharacterComponent>(entity).Velocity;
        auto& transform = view.get<TransformComponent>(entity);

        if (registry.all_of<CameraComponent>(entity))
        {
            auto& cameraController = *registry.get<CameraComponent>(entity).CameraController;
            cameraController.GetCamera().SetPosition(transform.Position);

            velocity = cameraController.CalculateMovementDirection();
            characterController.HandleInput(JoltUtils::GlmToJoltVec3(velocity), Input::IsKeyPressed(VE_KEY_SPACE), ts);
        }

        physicsCharacterManager.UpdateCharacterVirtual(characterController.GetCharacter(),
                                                       ts,
                                                       characterController.m_GravityStrength);
        UpdateTransformComponent(transform, characterController);
        RaiseColliderLocationChangedEvent(transform, *m_State.Application);
    }
}

static void UpdateTransformComponent(TransformComponent& transform, const BodyID& bodyId)
{
    const BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
    const Vec3 position = bodyInterface.GetCenterOfMassPosition(bodyId);
    Vec3 axis{};
    float_t angle;
    bodyInterface.GetRotation(bodyId).GetAxisAngle(axis, angle);
    UpdateTransformComponent(transform, JoltUtils::JoltToGlmVec3(position), angle, JoltUtils::JoltToGlmVec3(axis));
}

static void UpdateTransformComponent(TransformComponent& transform, const CharacterController& controller)
{
    const Vec3 position = controller.GetCharacter().GetPosition();
    Vec3 axis{};
    float_t angle;
    controller.GetCharacter().GetRotation().GetAxisAngle(axis, angle);
    UpdateTransformComponent(transform, JoltUtils::JoltToGlmVec3(position), angle, JoltUtils::JoltToGlmVec3(axis));
}

static void UpdateTransformComponent(TransformComponent& transform,
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

static void RaiseColliderLocationChangedEvent(const TransformComponent& transform, Application& application)
{
    if (static_cast<glm::ivec3>(glm::round(transform.PreviousPosition)) !=
        static_cast<glm::ivec3>(glm::round(transform.Position)))
    {
        ColliderLocationChangedEvent event(transform.Position);
        application.RaiseEvent(event);
    }
}

static void CalculateModelMatrix(glm::mat4 model, TransformComponent& transform)
{
    model = glm::translate(model, transform.Position);
    model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
    model = glm::scale(model, transform.Scale);
    transform.World = model;
}

static void UpdateTransformRecursive(const entt::entity entity, const TransformComponent& parentTransform)
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    auto transform = registry.try_get<TransformComponent>(entity);
    if (transform == nullptr)
        return;

    // auto collider = registry.try_get<ColliderComponent>(entity);

    // if (collider != nullptr)
    // {
    //     UpdateTransformComponent(transform, collider->BodyId);
    //     UpdateTransformComponent(transform,
    //                              transform.Position - parentTransform.Position,
    //                              transform.RotationAngle - parentTransform.RotationAngle,
    //                              transform.RotationAxis - parentTransform.RotationAxis);
    // }

    // transform += parentTransform;
    // UpdateTransformComponent(transform, transform.Position);

    CalculateModelMatrix(parentTransform.World, *transform);

    const auto children = registry.try_get<ChildrenComponent>(entity);
    if (children == nullptr)
        return;

    for (const auto& child: children->Entities)
        UpdateTransformRecursive(child, *transform);
}

}

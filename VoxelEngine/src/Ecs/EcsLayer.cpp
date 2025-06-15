#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Physics/Utils/JoltUtils.hpp"
#include "../Renderer/Renderer.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/CharacterComponent.hpp"
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
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto view = registry.view<ColliderComponent, TransformComponent>(); const auto entity: view)
    {
        const auto& collider = view.get<ColliderComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);
        if (!bodyInterface.IsActive(collider.BodyId))
            continue;
        UpdateTranslationComponentFromBody(collider.BodyId, transform);
        RaiseColliderLocationChangedEvent(transform);
    }

    for (const auto view = registry.view<TransformComponent, CharacterComponent>(); const auto entity: view)
    {
        auto& character = *view.get<CharacterComponent>(entity).Character;
        auto& transform = view.get<TransformComponent>(entity);
        const auto& bodyId = character.GetBodyID();

        character.Activate();
        character.PostSimulation(0.05f);
        UpdateTranslationComponentFromBody(bodyId, transform);

        if (registry.all_of<CameraComponent>(entity))
        {
            auto& controller = *registry.get<CameraComponent>(entity).CameraController;
            const auto& movement = controller.CalculateMovementVector(ts);
            glm::vec3 v = JoltUtils::JoltToGlmVec3(character.GetLinearVelocity());

            glm::vec2 xz(0.0f);
            if (movement.x != 0.0f || movement.z != 0.0f)
                xz = 10.0f * glm::normalize(glm::vec2(movement.x, movement.z));
            else if (glm::abs(v.x) > 1.0f || glm::abs(v.z) > 1.0f)
                xz = glm::normalize(glm::vec2(v.x, v.z));

            v.x = xz.x;
            v.z = xz.y;

            if (Input::IsKeyPressed(HZ_KEY_SPACE) && character.GetGroundState() == Character::EGroundState::OnGround)
                v.y = 10.0f;

            character.SetLinearVelocity(JoltUtils::GlmToJoltVec3(v));
            controller.GetCamera().SetPosition(transform.Position);
        }

        RaiseColliderLocationChangedEvent(transform);
    }

    Renderer::Instance().RenderScene(m_State.CameraController->GetCamera());
}

void EcsLayer::UpdateTranslationComponentFromBody(const BodyID& bodyId, TransformComponent& transform)
{
    BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();

    const RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyId);
    Vec3 r{};
    float_t angle;
    bodyInterface.GetRotation(bodyId).GetAxisAngle(r, angle);

    transform.PreviousPosition = transform.Position;
    transform.Position = glm::vec3(p.GetX(), p.GetY(), p.GetZ());
    transform.RotationAngle = angle;
    if (r != Vec3(0.0f, 0.0f, 0.0f))
        transform.RotationAxis = glm::vec3(r.GetX(), r.GetY(), r.GetZ());
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

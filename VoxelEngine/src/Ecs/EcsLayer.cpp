#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Utils/Utils.hpp"
#include "Components/CameraComponent.hpp"

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

void EcsLayer::OnUpdate(Timestep ts)
{
    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
    const BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const auto colliderTransformView = registry.view<ColliderComponent, TransformComponent>();
    for (const auto entity: colliderTransformView)
    {
        const auto& collider = colliderTransformView.get<ColliderComponent>(entity);
        auto bodyId = collider.BodyId;
        if (!bodyInterface.IsActive(bodyId))
            continue;
        RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyId);
        Vec3 r{};
        float_t angle;
        bodyInterface.GetRotation(bodyId).GetAxisAngle(r, angle);

        auto& transform = colliderTransformView.get<TransformComponent>(entity);
        transform.PreviousPosition = transform.Position;
        transform.Position = glm::vec3(p.GetX(), p.GetY(), p.GetZ());
        transform.RotationAngle = angle;
        if (r != Vec3(0.0f, 0.0f, 0.0f))
            transform.RotationAxis = glm::vec3(r.GetX(), r.GetY(), r.GetZ());
        else
            transform.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        if (static_cast<glm::ivec3>(glm::round(transform.PreviousPosition)) != static_cast<glm::ivec3>(glm::round(
                transform.Position)))
        {
            ColliderLocationChangedEvent event(transform.Position);
            m_State.Application->RaiseEvent(event);
        }
    }

    const auto cameraTransformView = registry.view<CameraComponent, TransformComponent>();
    for (const auto entity: cameraTransformView)
    {
        const auto& cameraComponent = cameraTransformView.get<CameraComponent>(entity);
        if (cameraComponent.CameraController->IsFreeFly())
            continue;
        const auto& transform = cameraTransformView.get<TransformComponent>(entity);
        auto& camera = cameraComponent.CameraController->GetCamera();
        camera.SetPosition(transform.Position);
    }

    Renderer::Instance().RenderScene(m_State.CameraController->GetCamera());
}

}

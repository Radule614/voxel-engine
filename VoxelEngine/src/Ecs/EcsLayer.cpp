#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Renderer/Renderer.hpp"
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

void EcsLayer::OnUpdate(const Timestep ts)
{
    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
    BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto view = registry.view<ColliderComponent, TransformComponent>(); const auto entity: view)
    {
        const auto& collider = view.get<ColliderComponent>(entity);
        auto bodyId = collider.BodyId;
        if (!bodyInterface.IsActive(bodyId))
            continue;
        RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyId);
        Vec3 r{};
        float_t angle;
        bodyInterface.GetRotation(bodyId).GetAxisAngle(r, angle);

        auto& transform = view.get<TransformComponent>(entity);
        transform.PreviousPosition = transform.Position;
        transform.Position = glm::vec3(p.GetX(), p.GetY(), p.GetZ());
        transform.RotationAngle = angle;
        if (r != Vec3(0.0f, 0.0f, 0.0f))
            transform.RotationAxis = glm::vec3(r.GetX(), r.GetY(), r.GetZ());
        else
            transform.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        if (registry.all_of<CameraComponent>(entity))
        {
            auto& controller = *registry.get<CameraComponent>(entity).CameraController;
            const auto& movementVector = controller.CalculateMovementVector(ts);
            Vec3 currentVelocity = bodyInterface.GetLinearVelocity(bodyId);
            auto xz = glm::vec2(movementVector.x, movementVector.z);
            if (xz != glm::vec2(0.0f, 0.0f))
            {
                xz = 10.0f * glm::normalize(xz);

                if (glm::abs(currentVelocity.GetY()) < 0.1f)
                {
                    bodyInterface.SetLinearVelocity(bodyId, Vec3(xz.x, -0.1f, xz.y));
                }
                else { bodyInterface.AddLinearVelocity(bodyId, Vec3(xz.x / 1000.0f, 0.0f, xz.y / 1000.0f)); }
            }
            controller.GetCamera().SetPosition(transform.Position + glm::vec3(0.0f, 0.7f, 0.0f));
        }

        if (static_cast<glm::ivec3>(glm::round(transform.PreviousPosition)) !=
            static_cast<glm::ivec3>(glm::round(transform.Position)))
        {
            ColliderLocationChangedEvent event(transform.Position);
            m_State.Application->RaiseEvent(event);
        }
    }

    // for (const auto view = registry.view<CameraComponent, TransformComponent>(); const auto entity: view)
    // {
    //     const auto& cameraComponent = view.get<CameraComponent>(entity);
    //     auto& transform = view.get<TransformComponent>(entity);
    //     auto& cameraController = *cameraComponent.CameraController;
    //     auto& camera = cameraController.GetCamera();
    //
    //     glm::vec3 movementVector = cameraController.CalculateMovementVector(ts);
    //     if (movementVector == glm::vec3(0.0f, 0.0f, 0.0f))
    //         continue;
    //
    //     if (registry.all_of<ColliderComponent>(entity))
    //     {
    //         glm::vec3 normalizedMovement = glm::normalize(glm::vec3(movementVector.x, 0.0f, movementVector.z));
    //         camera.SetPosition(transform.Position + normalizedMovement);
    //     }
    //     else
    //     {
    //         transform.Position += movementVector;
    //         camera.SetPosition(transform.Position);
    //     }
    // }

    Renderer::Instance().RenderScene(m_State.CameraController->GetCamera());
}

}

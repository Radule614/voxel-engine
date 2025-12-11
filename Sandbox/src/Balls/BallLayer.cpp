#include "BallLayer.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Assets/AssetManager.hpp"
#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MeshComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Enemy/Enemy.hpp"
#include "Physics/Utils/BodyBuilder.hpp"
#include "Physics/Character/CharacterBuilder.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace VoxelEngine;

namespace Sandbox
{

BallLayer::BallLayer(EngineState& state) : m_State(state)
{
    m_Model = AssetManager::Instance().LoadModel("assets/models/sphere/Sphere.glb");
}

BallLayer::~BallLayer() = default;

void BallLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TransformComponent transform{};
    transform.Scale = glm::vec3(1.0f);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto entity = registry.create();
    registry.emplace<TransformComponent>(entity, transform);

    const auto& cameraController = m_State.CameraController;
    if (!cameraController->IsFreeFly())
    {
        auto character = CharacterBuilder()
                .SetPosition(cameraController->GetCamera().GetPosition())
                .BuildAndAddVirtual();
        auto characterController = std::make_unique<CharacterController>(std::move(character));

        registry.emplace<CharacterComponent>(entity, std::move(characterController));
        registry.emplace<CameraComponent>(entity, cameraController);
    }

    new Enemy(cameraController->GetCamera().GetPosition());
}

void BallLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) {
            if (e.GetKeyCode() == VE_KEY_T)
            {
                const PerspectiveCamera& camera = m_State.CameraController->GetCamera();
                const glm::vec3 front = camera.GetFront();
                const glm::vec3 position = camera.GetPosition();

                const ShapeRefC shape{ShapeFactory().CreateSphereShape(0.4f)};
                const BodyID bodyId = BodyBuilder()
                        .SetShape(shape)
                        .SetPosition(position)
                        .SetMotionType(EMotionType::Dynamic)
                        .SetActivation(EActivation::Activate)
                        .BuildAndAdd();

                BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
                bodyInterface.AddLinearVelocity(bodyId, 20 * Vec3(front.x, front.y, front.z));
                TransformComponent transform{};
                transform.Scale = glm::vec3(0.4);

                auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
                const auto entity = registry.create();
                registry.emplace<MeshComponent>(entity, std::shared_ptr<Model>(m_Model));
                registry.emplace<TransformComponent>(entity, transform);
                registry.emplace<ColliderComponent>(entity, ColliderComponent(bodyId));
                m_SphereEntities.emplace_back(entity, 0);
            }
            return false;
        });
}

void BallLayer::OnUpdate(const Timestep ts)
{
    for (auto it = m_SphereEntities.begin(); it != m_SphereEntities.end();)
    {
        const entt::entity& sphere = it->first;
        float_t& accumulatedTime = it->second;
        accumulatedTime += ts;
        if (accumulatedTime > 10.0f)
        {
            BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
            auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
            auto& collider = registry.view<ColliderComponent>().get<ColliderComponent>(sphere);
            bodyInterface.RemoveBody(collider.BodyId);
            bodyInterface.DestroyBody(collider.BodyId);
            registry.destroy(sphere);
            it = m_SphereEntities.erase(it);
        }
        else
            ++it;
    }
}

}

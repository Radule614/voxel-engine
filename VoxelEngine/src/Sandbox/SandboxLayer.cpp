#include "SandboxLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Ecs/Components/CameraComponent.hpp"
#include "../Physics/Builders/BodyBuilder.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace VoxelEngine;

namespace Sandbox
{

SandboxLayer::SandboxLayer(EngineState& state) : m_State(state)
{
    m_Shader = std::make_shared<Shader>(
        *Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl"));
    m_TextureAtlas = AssetManager::Instance().LoadTexture("assets/textures/atlas.png", "Diffuse");
    m_Model = AssetManager::Instance().LoadModel("assets/models/sphere/sphere.obj");
}

SandboxLayer::~SandboxLayer() = default;

void SandboxLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto& cameraController = m_State.CameraController;
    const ShapeRefC shape = ShapeFactory().CreateCapsuleShape(1.0f, 0.5f);
    const BodyID bodyId = BodyBuilder()
            .SetShape(shape)
            .SetPosition(cameraController->GetCamera().GetPosition())
            .SetMotionType(EMotionType::Dynamic)
            .SetActivation(EActivation::Activate)
            .SetConstraints(EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ)
            .SetAllowSleeping(false)
            .BuildAndAdd();

    TransformComponent transform{};
    transform.Scale = glm::vec3(1.0f);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto entity = registry.create();
    registry.emplace<TransformComponent>(entity, transform);
    registry.emplace<ColliderComponent>(entity, ColliderComponent(bodyId));
    registry.emplace<CameraComponent>(entity, cameraController);
}

void SandboxLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& e) {
            if (e.GetKeyCode() == HZ_KEY_T)
            {
                const PerspectiveCamera& camera = m_State.CameraController->GetCamera();
                const glm::vec3 front = camera.GetFront();
                const glm::vec3 position = camera.GetPosition();

                const ShapeRefC shape = ShapeFactory().CreateSphereShape(0.4f);
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
                registry.emplace<MeshComponent>(entity, m_Shader, m_Model->Meshes);
                registry.emplace<TransformComponent>(entity, transform);
                registry.emplace<ColliderComponent>(entity, ColliderComponent(bodyId));
                m_SphereEntities.emplace_back(entity, 0);
            }
            return false;
        });
}

void SandboxLayer::OnUpdate(const Timestep ts)
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

#include "PlayerScript.hpp"

#include "Assets/AssetManager.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/ModelEntity.hpp"
#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/ColliderComponent.hpp"
#include "Ecs/Components/MetadataComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Physics/Utils/BodyBuilder.hpp"
#include "Physics/Utils/ShapeFactory.hpp"
#include "GLCore/Core/Input.hpp"
#include "GLCore/Events/KeyEvent.hpp"

#include "Jolt/Physics/Collision/NarrowPhaseQuery.h"
#include "Jolt/Physics/Collision/CollideShape.h"
#include "Jolt/Physics/Collision/CollisionCollectorImpl.h"
#include "Jolt/Physics/Body/BodyFilter.h"

using namespace VoxelEngine;
using namespace GLCore;

namespace Expedition
{

PlayerScript::PlayerScript() : Script("Player Script")
{
}

void PlayerScript::OnUpdate(const Timestep ts, ScriptContext context)
{
    auto& registry = context.Registry;
    auto* charComp = registry.try_get<CharacterComponent>(context.Entity);
    if (!charComp)
        return;

    auto& controller = *charComp->Controller;

    // --- Player attack (LMB) movement lock ---
    if (m_Attacking)
    {
        m_AttackTimer -= ts.GetSeconds();
        if (m_AttackTimer <= 0.0f)
        {
            m_Attacking                 = false;
            controller.m_CharacterSpeed = m_SavedSpeed;
        }
    }
    else if (Input::IsMouseButtonPressed(0))
    {
        m_Attacking                 = true;
        m_AttackTimer               = m_AttackDuration;
        m_SavedSpeed                = controller.m_CharacterSpeed;
        controller.m_CharacterSpeed = 0.0f;
    }

    // --- Ball collision check and cleanup ---
    auto& physSystem = PhysicsEngine::Instance().GetSystem();
    auto& bi         = physSystem.GetBodyInterface();

    for (auto it = m_Balls.begin(); it != m_Balls.end();)
    {
        const JPH::RMat44 transform = bi.GetCenterOfMassTransform(it->BodyId);
        const JPH::RefConst<JPH::Shape> shape = bi.GetShape(it->BodyId);

        JPH::AllHitCollisionCollector<JPH::CollideShapeCollector> collector;
        JPH::IgnoreSingleBodyFilter bodyFilter(it->BodyId);
        physSystem.GetNarrowPhaseQuery().CollideShape(
            shape, JPH::Vec3::sReplicate(1.0f), transform,
            JPH::CollideShapeSettings{}, JPH::RVec3::sZero(),
            collector, {}, {}, bodyFilter);

        if (!collector.mHits.empty())
        {
            bi.RemoveBody(it->BodyId);
            bi.DestroyBody(it->BodyId);
            EntityComponentSystem::Instance().DestroyEntityRecursive(it->Entity);
            it = m_Balls.erase(it);
        }
        else
            ++it;
    }
}

void PlayerScript::OnEvent(Event& event, ScriptContext context)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>([&](const KeyPressedEvent& e)
    {
        if (e.GetKeyCode() != VE_KEY_T)
            return false;

        auto* camComp = context.Registry.try_get<CameraComponent>(context.Entity);
        if (!camComp)
            return false;

        const auto&      camera   = camComp->CameraController->GetCamera();
        const glm::vec3  front    = camera.GetFront();
        const glm::vec3  spawnPos = camera.GetPosition() + front * 2.0f;

        const JPH::ShapeRefC shape{ ShapeFactory().CreateSphereShape(0.4f) };
        const JPH::BodyID bodyId = BodyBuilder()
            .SetShape(shape)
            .SetPosition(spawnPos)
            .SetMotionType(JPH::EMotionType::Dynamic)
            .SetActivation(JPH::EActivation::Activate)
            .BuildAndAdd();

        JPH::BodyInterface& bi = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
        bi.AddLinearVelocity(bodyId, 20.0f * JPH::Vec3(front.x, front.y, front.z));

        const auto ballEntity = CreateEntityFromModel(AssetManager::Instance().GetSphereModel());
        context.Registry.emplace<ColliderComponent>(ballEntity, bodyId, shape->GetType(), shape->GetSubType());
        context.Registry.get<MetadataComponent>(ballEntity).Name = "Ball";

        auto& t = context.Registry.get<TransformComponent>(ballEntity);
        t.LocalScale = glm::vec3(0.4f);
        t.IsDirty    = true;

        m_Balls.push_back({ ballEntity, bodyId });
        return false;
    });
}

void PlayerScript::OnDetach(ScriptContext context)
{
    auto& bi = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
    for (const auto& ball : m_Balls)
    {
        bi.RemoveBody(ball.BodyId);
        bi.DestroyBody(ball.BodyId);
        EntityComponentSystem::Instance().DestroyEntityRecursive(ball.Entity);
    }
    m_Balls.clear();
}

}

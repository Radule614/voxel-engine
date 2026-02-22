//
// Created by RadU on 2/5/2026.
//

#include "EcsLayer.hpp"

#include "Ecs.hpp"
#include "Components/ParentComponent.hpp"
#include "glm/gtc/quaternion.hpp"

namespace VoxelEngine
{

static void CalculateTransform(TransformComponent& transform, const TransformComponent& parentTransform);
static void UpdateTransformRecursive(entt::entity entity, const TransformComponent& parentTransform);

EcsLayer::EcsLayer(EngineState& state) : m_State(state)
{
}

void EcsLayer::OnAttach() { Layer::OnAttach(); }

static std::tuple<int32_t, int32_t, float_t> FindAnimationIndices(const std::vector<float>& times,
                                                                  const float_t currentTime)
{
    const auto it = std::ranges::upper_bound(times, currentTime);
    int nextIndex = std::distance(times.begin(), it);

    if (nextIndex >= times.size())
        nextIndex = times.size() - 1;

    int previousIndex = std::max(0, nextIndex - 1);

    const float t0 = times[previousIndex];
    const float t1 = times[nextIndex];

    float alpha = (t1 > t0) ? (currentTime - t0) / (t1 - t0) : 0.0f;

    return {previousIndex, nextIndex, alpha};
}

static glm::vec3 CubicVec3(
    const glm::vec3& p0,
    const glm::vec3& m0,
    const glm::vec3& p1,
    const glm::vec3& m1,
    const float_t t,
    const float_t dt)
{
    const float_t t2 = t * t;
    const float_t t3 = t2 * t;

    const float_t h00 = 2 * t3 - 3 * t2 + 1;
    const float_t h10 = t3 - 2 * t2 + t;
    const float_t h01 = -2 * t3 + 3 * t2;
    const float_t h11 = t3 - t2;

    return h00 * p0 + h10 * (dt * m0) + h01 * p1 + h11 * (dt * m1);
}

static glm::quat CubicQuat(
    const glm::quat& p0,
    const glm::quat& m0,
    const glm::quat& p1,
    const glm::quat& m1,
    const float_t t,
    const float_t dt)
{
    const float_t t2 = t * t;
    const float_t t3 = t2 * t;

    const float_t h00 = 2 * t3 - 3 * t2 + 1;
    const float_t h10 = t3 - 2 * t2 + t;
    const float_t h01 = -2 * t3 + 3 * t2;
    const float_t h11 = t3 - t2;

    glm::vec4 P0(p0.x, p0.y, p0.z, p0.w);
    glm::vec4 M0(m0.x, m0.y, m0.z, m0.w);
    glm::vec4 P1(p1.x, p1.y, p1.z, p1.w);
    glm::vec4 M1(m1.x, m1.y, m1.z, m1.w);

    glm::vec4 r = h00 * P0 + h10 * (dt * M0) + h01 * P1 + h11 * (dt * M1);

    return glm::normalize(glm::quat(r.w, r.x, r.y, r.z));
}

static void AdvanceNodeAnimation(const entt::entity& nodeEntity,
                                 const NodeAnimation& nodeAnimation,
                                 const float_t currentTime)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    auto& transform = registry.get<TransformComponent>(nodeEntity);
    transform.IsDirty = true;

    for (const auto& [Target, Interpolation, Times, VectorValues, QuatValues]: nodeAnimation.Tracks)
    {
        if (currentTime > Times.back())
            continue;

        auto [previous, next, alpha] = FindAnimationIndices(Times, currentTime);

        if (Interpolation == Step)
            alpha = glm::round(alpha);

        if (Interpolation == Linear || Interpolation == Step)
        {
            if (Target == Translation)
                transform.LocalPosition = glm::mix(VectorValues[previous], VectorValues[next], alpha);
            else if (Target == Rotation)
                transform.LocalRotation = glm::normalize(glm::slerp(QuatValues[previous], QuatValues[next], alpha));
            else if (Target == Scale)
                transform.LocalScale = glm::mix(VectorValues[previous], VectorValues[next], alpha);

            continue;
        }

        const float_t dt = Times[next] - Times[previous];

        const int32_t pBase = previous * 3;
        const int32_t nBase = next * 3;

        if (Target == Translation || Target == Scale)
        {
            const glm::vec3& p0 = VectorValues[pBase + 1];
            const glm::vec3& m0 = VectorValues[pBase + 2];
            const glm::vec3& p1 = VectorValues[nBase + 1];
            const glm::vec3& m1 = VectorValues[nBase + 0];

            const glm::vec3 result = CubicVec3(p0, m0, p1, m1, alpha, dt);

            if (Target == Translation)
                transform.LocalPosition = result;
            else
                transform.LocalScale = result;
        }
        else if (Target == Rotation)
        {
            const glm::quat& p0 = QuatValues[pBase + 1];
            const glm::quat& m0 = QuatValues[pBase + 2];
            const glm::quat& p1 = QuatValues[nBase + 1];
            const glm::quat& m1 = QuatValues[nBase + 0];

            transform.LocalRotation = glm::normalize(CubicQuat(p0, m0, p1, m1, alpha, dt));
        }
    }
}

static void UpdateAnimations(const GLCore::Timestep ts)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const auto& animatedEntities = registry.view<AnimationComponent>();

    for (const entt::entity& animatedEntity: animatedEntities)
    {
        std::vector<EntityAnimation>& animations = registry.get<AnimationComponent>(animatedEntity).Animations;

        for (auto& animation: animations)
        {
            if (!animation.IsActive)
                continue;

            animation.Time += ts;

            for (const auto& [nodeEntity, nodeAnimation]: animation.NodeAnimations)
                AdvanceNodeAnimation(nodeEntity, nodeAnimation, animation.Time);

            if (animation.Time > animation.Duration)
            {
                animation.Time = 0.0f;

                if (!animation.ShouldRepeat)
                    animation.IsActive = false;
            }
        }
    }
}

static void UpdateTransforms()
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const auto rootView = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto rootEntity: rootView)
        UpdateTransformRecursive(rootEntity, TransformComponent{});
}

void EcsLayer::OnUpdate(const GLCore::Timestep ts)
{
    UpdateAnimations(ts);
    UpdateTransforms();
}

static void CalculateTransform(TransformComponent& transform, const TransformComponent& parentTransform)
{
    if (transform.IsDirty)
    {
        transform.LocalMatrix = glm::translate(glm::mat4(1.0f), transform.LocalPosition) *
                                glm::mat4_cast(transform.LocalRotation) *
                                glm::scale(glm::mat4(1.0f), transform.LocalScale);

        transform.IsDirty = false;
    }

    transform.PreviousWorldPosition = glm::vec3(transform.WorldMatrix[3]);
    transform.WorldMatrix = parentTransform.WorldMatrix * transform.LocalMatrix;
    transform.WorldPosition = transform.WorldMatrix[3];
}

static void UpdateTransformRecursive(const entt::entity entity, const TransformComponent& parentTransform)
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    auto transform = registry.try_get<TransformComponent>(entity);
    if (transform == nullptr)
        return;

    CalculateTransform(*transform, parentTransform);

    const auto children = registry.try_get<ChildrenComponent>(entity);
    if (children == nullptr)
        return;

    for (const auto& child: children->Entities)
        UpdateTransformRecursive(child, *transform);
}

}

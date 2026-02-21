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

    return std::tuple(previousIndex, nextIndex, alpha);
}

static void AdvanceNodeAnimation(const entt::entity& nodeEntity,
                                 const NodeAnimation& nodeAnimation,
                                 const float_t currentTime)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    auto& transform = registry.get<TransformComponent>(nodeEntity);
    transform.IsDirty = true;

    for (const auto& track: nodeAnimation.Tracks)
    {
        if (currentTime > track.Times.back())
            continue;

        const auto& times = track.Times;
        const auto& vectorValues = track.VectorValues;
        const auto& quatValues = track.QuatValues;

        const auto [previous, next, alpha] = FindAnimationIndices(times, currentTime);

        if (track.Target == Translation)
            transform.LocalPosition = glm::mix(vectorValues[previous], vectorValues[next], alpha);
        else if (track.Target == Rotation)
            transform.LocalRotation = glm::normalize(glm::slerp(quatValues[previous], quatValues[next], alpha));
        else if (track.Target == Scale)
            transform.LocalScale = glm::mix(vectorValues[previous], vectorValues[next], alpha);
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
                animation.IsActive = false;
                animation.Time = 0.0f;
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

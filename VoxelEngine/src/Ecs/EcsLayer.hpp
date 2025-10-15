#pragma once

#include <GLCoreUtils.hpp>
#include "../EngineState.hpp"
#include "Components/CharacterComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace VoxelEngine
{

class EcsLayer : public GLCore::Layer
{
public:
    explicit EcsLayer(EngineState& state);
    ~EcsLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    void UpdateTransformComponent(TransformComponent& transform, const JPH::BodyID& bodyId);
    void UpdateTransformComponent(TransformComponent& transform, const CharacterController& controller);
    void UpdateTransformComponent(TransformComponent& transform,
                                  glm::vec3 position,
                                  float_t angle,
                                  glm::vec3 axis);
    void RaiseColliderLocationChangedEvent(const TransformComponent& transform);

private:
    EngineState& m_State;
};

}

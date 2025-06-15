#pragma once
#include "GLCore.hpp"
#include <GLCoreUtils.hpp>
#include "../EngineState.hpp"
#include "../Ecs/Ecs.hpp"

namespace VoxelEngine
{

class EcsLayer : public GLCore::Layer
{
public:
    EcsLayer(EngineState& state);
    ~EcsLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(GLCore::Timestep ts) override;

private:
    void UpdateTransformComponent(TransformComponent& transform, const JPH::BodyID& bodyId);
    void UpdateTransformComponent(TransformComponent& transform, const JPH::CharacterVirtual& character);
    void UpdateTransformComponent(TransformComponent& transform,
                                  glm::vec3 position,
                                  float_t angle,
                                  glm::vec3 axis);
    void RaiseColliderLocationChangedEvent(const TransformComponent& transform);

private:
    EngineState& m_State;
};

}

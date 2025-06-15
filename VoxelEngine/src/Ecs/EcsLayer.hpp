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
    void UpdateTranslationComponentFromBody(const JPH::BodyID& bodyId, TransformComponent& transform);
    void RaiseColliderLocationChangedEvent(const TransformComponent& transform);

private:
    EngineState& m_State;
};

}

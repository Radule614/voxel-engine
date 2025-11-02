#pragma once

#include "EngineState.hpp"
#include "Assets/Model.hpp"
#include "Ecs/Ecs.hpp"
#include "Physics/Utils/ShapeFactory.hpp"

namespace Sandbox
{

class BallLayer : public GLCore::Layer
{
public:
    explicit BallLayer(VoxelEngine::EngineState& state);
    ~BallLayer() override;

    virtual void OnAttach() override;
    virtual void OnEvent(GLCore::Event& event) override;
    virtual void OnUpdate(GLCore::Timestep ts) override;

private:
    VoxelEngine::EngineState& m_State;
    VoxelEngine::Model* m_Model;
    std::vector<std::pair<entt::entity, float_t> > m_SphereEntities{};
    VoxelEngine::ShapeFactory m_PhysicsFactory;
};

}

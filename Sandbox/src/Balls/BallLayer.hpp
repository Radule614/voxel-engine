#pragma once

#include "EngineState.hpp"
#include "Assets/Gltf/Model.hpp"
#include "Ecs/Ecs.hpp"
#include "Physics/Utils/ShapeFactory.hpp"

namespace Sandbox
{

class BallLayer : public GLCore::Layer
{
public:
    explicit BallLayer(VoxelEngine::EngineState& state);
    ~BallLayer() override;

    void OnAttach() override;
    void OnEvent(GLCore::Event& event) override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    VoxelEngine::EngineState& m_State;
    std::vector<std::pair<entt::entity, float_t> > m_SphereEntities{};
    VoxelEngine::ShapeFactory m_PhysicsFactory;
};

}

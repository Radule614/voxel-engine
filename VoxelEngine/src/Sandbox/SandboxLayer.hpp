#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "../EngineState.hpp"
#include "../Assets/Model.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Physics/Utils/ShapeFactory.hpp"

//TODO: This should be a different project, and it should be using Voxel Engine as the dependency
namespace Sandbox
{

class SandboxLayer : public GLCore::Layer
{
public:
    explicit SandboxLayer(VoxelEngine::EngineState& state);
    ~SandboxLayer() override;

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

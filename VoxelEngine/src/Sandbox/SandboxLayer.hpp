#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "../EngineState.hpp"
#include "../Assets/Model.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Physics/Builders/ShapeFactory.hpp"

//TODO: This should be a different project, and it should be using Voxel Engine as the dependency
namespace Sandbox
{

class SandboxLayer : public GLCore::Layer
{
public:
    SandboxLayer(VoxelEngine::EngineState& state);
    ~SandboxLayer();

    virtual void OnAttach() override;
    virtual void OnEvent(GLCore::Event& event) override;
    virtual void OnUpdate(GLCore::Timestep ts) override;

private:
    VoxelEngine::EngineState& m_State;
    std::shared_ptr<GLCore::Utils::Shader> m_Shader;
    VoxelEngine::Texture m_TextureAtlas;
    VoxelEngine::Model* m_Model;
    std::vector<std::pair<entt::entity, float_t> > m_SphereEntities{};
    VoxelEngine::ShapeFactory m_PhysicsFactory;
};

}

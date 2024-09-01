#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "../EngineState.hpp"
#include "../Assets/Model.hpp"

namespace VoxelEngine
{

class PhysicsLayer : public GLCore::Layer
{
public:
	PhysicsLayer(EngineState& state);
	~PhysicsLayer();

	virtual void OnAttach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
private:
	EngineState& m_State;
	std::shared_ptr<GLCore::Utils::Shader> m_Shader;
	VoxelEngine::Texture m_TextureAtlas;
	Model* m_Model;
};

}

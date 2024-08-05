#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "../EngineState.hpp"

namespace VoxelEngine
{

struct RenderMetadata
{
	GLuint VertexArray;
	GLuint VertexBuffer;
	GLuint IndexBuffer;
	std::vector<uint32_t> Indices;
	glm::mat4 ModelMatrix;
};

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
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::Texture m_TextureAtlas;
	RenderMetadata m_RenderMetadata;
};

}

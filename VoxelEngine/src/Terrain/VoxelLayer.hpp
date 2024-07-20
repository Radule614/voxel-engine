#pragma once

#include <unordered_map>

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "World.hpp"
#include "../EngineState.hpp"

namespace VoxelEngine
{

struct ChunkRenderMetadata
{
	GLuint VertexArray;
	GLuint VertexBuffer;
	GLuint IndexBuffer;
	std::vector<uint32_t> Indices;
	glm::mat4 ModelMatrix;
};

class VoxelLayer : public GLCore::Layer
{
public:
	VoxelLayer(const EngineState& state);
	~VoxelLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

private:
	void CheckChunkRenderQueue();
	void SetupRenderData(std::shared_ptr<Chunk> chunk);

private:
	const EngineState& m_State;
	GLCore::Utils::PerspectiveCameraController m_CameraController;
	GLCore::Utils::Shader* m_Shader;


	World m_World;
	std::unordered_map<Position2D, ChunkRenderMetadata> m_RenderMetadata;

	//TEMP
	GLCore::Utils::TextureManager m_TextureManager;
	GLCore::Utils::Texture m_TextureAtlas;
};

};

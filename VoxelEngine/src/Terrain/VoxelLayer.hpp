#pragma once

#include <unordered_map>

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "World.hpp"
#include "../EngineState.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Renderer/Renderer.hpp"

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
	VoxelLayer(EngineState& state);
	~VoxelLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

private:
	struct UIState
	{
		int32_t ThreadCount = TerrainConfig::ThreadCount - 1;
		int32_t PolygonMode = TerrainConfig::PolygonMode == GL_FILL ? 0 : 1;
	};

	void CheckChunkRenderQueue();
	void SetupRenderData(std::shared_ptr<Chunk> chunk);
	void ApplyState() const;
	void OnColliderLocationChanged(glm::vec3 pos);
	void OptimizeColliders();

private:
	EngineState& m_EngineState;
	UIState m_UIState;
	GLCore::Utils::Shader* m_Shader;
	VoxelEngine::Texture m_TextureAtlas;
	World m_World;
	std::unordered_map<Position2D, ChunkRenderMetadata> m_RenderMetadata;
	JPH::ShapeRefC m_VoxelShape;
	std::unordered_map<glm::i16vec3, ColliderComponent> m_VoxelColliders;
	float_t timeSinceLastColliderOptimization = 0.0f;
};

};

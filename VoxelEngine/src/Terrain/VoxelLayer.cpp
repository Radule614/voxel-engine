#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "../Physics/PhysicsEngine.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace JPH::literals;


namespace VoxelEngine
{

VoxelLayer::VoxelLayer(const EngineState& engineState)
	: Layer("VoxelLayer"), m_EngineState(engineState), m_CameraController(45.0f, 16.0f / 9.0f, 150.0f), m_RenderMetadata({}), m_World(World(m_CameraController)), m_TextureManager()
{
	m_CameraController.GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));
}

VoxelLayer::~VoxelLayer()
{
}

void VoxelLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
	m_TextureAtlas = m_TextureManager.LoadTexture("assets/textures/atlas.png", "texture_diffuse");

	m_World.StartGeneration();
}

void VoxelLayer::OnDetach()
{
	m_World.StopGeneration();
	for (auto it = m_RenderMetadata.begin(); it != m_RenderMetadata.end(); ++it)
	{
		ChunkRenderMetadata& metadata = it->second;
		glDeleteVertexArrays(1, &metadata.VertexArray);
		glDeleteBuffers(1, &metadata.VertexBuffer);
		glDeleteBuffers(1, &metadata.IndexBuffer);
		metadata.Indices.clear();
	}
	m_RenderMetadata.clear();
	delete m_Shader;
}

void VoxelLayer::OnEvent(GLCore::Event& event)
{
	if (!m_EngineState.MenuActive)
		m_CameraController.OnEvent(event);
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(
		[&](WindowCloseEvent& e)
		{
			m_World.StopGeneration();
			return false;
		});
	dispatcher.Dispatch<StatePauseEvent>(
		[&](StatePauseEvent& e)
		{
			m_World.StopGeneration();
			UIState state;
			m_UIState = state;
			return false;
		});
	dispatcher.Dispatch<StateUnpauseEvent>(
		[&](StateUnpauseEvent& e)
		{
			m_World.StartGeneration();
			return false;
		});
}

void VoxelLayer::OnUpdate(Timestep ts)
{
	if (!m_EngineState.MenuActive)
		m_CameraController.OnUpdate(ts);
	glClearColor(0.14f, 0.59f, 0.74f, 0.7f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_Shader->GetRendererID());

	CheckChunkRenderQueue();

	for (auto it = m_RenderMetadata.cbegin(); it != m_RenderMetadata.cend(); ++it)
	{
		const ChunkRenderMetadata& metadata = it->second;

		auto& viewMatrix = m_CameraController.GetCamera().GetViewProjectionMatrix();
		int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Model");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(metadata.ModelMatrix));

		glActiveTexture(GL_TEXTURE0);
		location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Atlas");
		glUniform1i(location, 0);
		glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

		glBindVertexArray(metadata.VertexArray);
		glDrawElements(GL_TRIANGLES, metadata.Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void VoxelLayer::OnImGuiRender()
{
	if (!m_EngineState.MenuActive)
		return;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize(ImVec2(400.0, 600.0));
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 400.0, 0));

	ImGui::Begin("Terrain Settings", nullptr, windowFlags);
	ImGui::Text("Terrain Settings");

	const char* polygonModes[] = { "Fill", "Line" };
	ImGui::Combo("Polygon Mode", &m_UIState.PolygonMode, polygonModes, IM_ARRAYSIZE(polygonModes));
	const char* threadCounts[] = { "1", "2", "3", "4" };
	ImGui::Combo("Thread Count", &m_UIState.ThreadCount, threadCounts, IM_ARRAYSIZE(threadCounts));

	if (ImGui::Button("Apply"))
		ApplyState();

	ImGui::End();
}

void VoxelLayer::ApplyState() const
{
	TerrainConfig::PolygonMode = m_UIState.PolygonMode == 0 ? GL_FILL : GL_LINE;
	TerrainConfig::ThreadCount = m_UIState.ThreadCount + 1;
	glPolygonMode(GL_FRONT_AND_BACK, TerrainConfig::PolygonMode);
}

void VoxelLayer::CheckChunkRenderQueue()
{
	auto& worldLock = m_World.GetLock();
	auto& chunks = m_World.GetChangedChunks();
	if (chunks.empty() || !worldLock.try_lock())
		return;
	auto it = chunks.begin();
	while (it != chunks.end())
	{
		std::shared_ptr<Chunk> chunk = *it;
		auto& chunkLock = chunk->GetLock();
		if (!chunkLock.try_lock())
		{
			++it;
			continue;
		}
		SetupRenderData(chunk);
		it = chunks.erase(it);

		//Temporary physics check
		LOG_INFO(chunk->GetPosition().ToString());
		
		PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
		BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

		/*
		BodyCreationSettings boxSettings(new BoxShape(Vec3(0.5f, 0.5f, 0.5f)), Vec3(0, 0, 0), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
		VoxelGrid& grid = chunk->GetVoxelGrid();
		for (size_t x = 0; x < CHUNK_WIDTH; ++x)
		{
			for (size_t z = 0; z < CHUNK_WIDTH; ++z)
			{
				for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
				{
					Voxel& v = grid[x][z][y];
					Vec3 p = Vec3(x, y, z);
					BodyID voxelId = bodyInterface.CreateAndAddBody(boxSettings, EActivation::DontActivate);
					bodyInterface.SetPosition(voxelId, p, EActivation::DontActivate);
					LOG_INFO("({0}, {1}, {2})", p.GetX(), p.GetY(), p.GetZ());
				}
			}
		}
		*/
		
		BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));
		floor_shape_settings.SetEmbedded();
		ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
		ShapeRefC floor_shape = floor_shape_result.Get();
		BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
		Body* floor = bodyInterface.CreateBody(floor_settings);
		bodyInterface.AddBody(floor->GetID(), EActivation::DontActivate);


		BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
		BodyID sphere_id = bodyInterface.CreateAndAddBody(sphere_settings, EActivation::Activate);
		bodyInterface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

		physicsSystem.OptimizeBroadPhase();

		//Temporary end

		chunkLock.unlock();
	}
	worldLock.unlock();
}

void VoxelLayer::SetupRenderData(std::shared_ptr<Chunk> chunk)
{
	ChunkRenderMetadata metadata = {};
	auto renderData = m_RenderMetadata.find(chunk->GetPosition());
	if (renderData != m_RenderMetadata.end())
	{
		ChunkRenderMetadata& m = renderData->second;
		glDeleteVertexArrays(1, &m.VertexArray);
		glDeleteBuffers(1, &m.VertexBuffer);
		glDeleteBuffers(1, &m.IndexBuffer);
		m.Indices.clear();
		m_RenderMetadata.erase(chunk->GetPosition());
	}

	std::vector<Vertex> vertices = {};
	vertices.insert(vertices.end(), chunk->GetMesh().begin(), chunk->GetMesh().end());
	vertices.insert(vertices.end(),
		chunk->GetBorderMesh(VoxelFace::FRONT).begin(),
		chunk->GetBorderMesh(VoxelFace::FRONT).end());
	vertices.insert(vertices.end(),
		chunk->GetBorderMesh(VoxelFace::RIGHT).begin(),
		chunk->GetBorderMesh(VoxelFace::RIGHT).end());
	vertices.insert(vertices.end(),
		chunk->GetBorderMesh(VoxelFace::BACK).begin(),
		chunk->GetBorderMesh(VoxelFace::BACK).end());
	vertices.insert(vertices.end(),
		chunk->GetBorderMesh(VoxelFace::LEFT).begin(),
		chunk->GetBorderMesh(VoxelFace::LEFT).end());

	if (vertices.empty())
		return;

	glCreateVertexArrays(1, &metadata.VertexArray);
	glBindVertexArray(metadata.VertexArray);

	glCreateBuffers(1, &metadata.VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, metadata.VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Texture)));

	std::vector<uint32_t> indices = {};
	uint32_t faceCount = vertices.size() / 4;
	for (uint32_t i = 0; i < faceCount; ++i)
	{
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4 + 0);
	}
	glCreateBuffers(1, &metadata.IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, metadata.IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	metadata.Indices = indices;
	metadata.ModelMatrix = chunk->GetModelMatrix();

	m_RenderMetadata.insert({ chunk->GetPosition(), metadata });
}

};

#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Terrain/Voxel.hpp"
#include "../Terrain/VoxelMeshBuilder.hpp"
#include "../Assets/Model.hpp"
#include "../Ecs/MeshComponent.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace JPH::literals;

static BodyID TestVoxelID;
static VoxelEngine::Voxel v(VoxelEngine::VoxelType::STONE);

namespace VoxelEngine
{

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
	m_Shader = std::make_shared<Shader>(*Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl"));
	m_TextureAtlas = m_State.AssetManager.LoadTexture("assets/textures/atlas.png", "texture_diffuse");
}

PhysicsLayer::~PhysicsLayer()
{
}

void PhysicsLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	BodyCreationSettings voxelSettings(new BoxShape(Vec3(1.0f, 1.0f, 1.0f)), RVec3(CHUNK_WIDTH / 2, CHUNK_HEIGHT + 10.0_r, 5), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	BodyID voxelId = bodyInterface.CreateAndAddBody(voxelSettings, EActivation::Activate);
	bodyInterface.AddImpulse(voxelId, Vec3(200.0f, 0.0f, 0.0f), Vec3(30.0f, 0.0f, 0.0f));
	TestVoxelID = voxelId;

	physicsSystem.OptimizeBroadPhase();

	Model model("assets/models/sphere/sphere.obj");
	MeshComponent mesh(m_Shader, model.GetMeshes());
	
	VoxelMeshBuilder meshBuilder;
	v.SetAllFacesVisible(true);
	std::vector<Vertex> vertices = meshBuilder.FromVoxel(v);

	glCreateVertexArrays(1, &m_RenderMetadata.VertexArray);
	glBindVertexArray(m_RenderMetadata.VertexArray);

	glCreateBuffers(1, &m_RenderMetadata.VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderMetadata.VertexBuffer);
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
	glCreateBuffers(1, &m_RenderMetadata.IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderMetadata.IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_RenderMetadata.Indices = indices;
	m_RenderMetadata.ModelMatrix = glm::translate(glm::mat4(1), glm::vec3(CHUNK_WIDTH / 2, CHUNK_HEIGHT + 10.0f, 5));
	m_RenderMetadata.ModelMatrix = glm::scale(m_RenderMetadata.ModelMatrix, glm::vec3(2));
}

void PhysicsLayer::OnEvent(GLCore::Event& event)
{
	if (!m_State.MenuActive)
		m_State.CameraController.OnEvent(event);
}

void PhysicsLayer::OnUpdate(GLCore::Timestep ts)
{
	PhysicsEngine::Instance().OnUpdate(ts);
	if (!m_State.MenuActive)
		m_State.CameraController.OnUpdate(ts);

	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	if (bodyInterface.IsActive(TestVoxelID))
	{
		RVec3 p = bodyInterface.GetCenterOfMassPosition(TestVoxelID);
		Vec3 v = bodyInterface.GetLinearVelocity(TestVoxelID);
		Vec3 r;
		float_t angle;
		bodyInterface.GetRotation(TestVoxelID).GetAxisAngle(r, angle);
		m_RenderMetadata.ModelMatrix = glm::translate(glm::mat4(1), glm::vec3(p.GetX(), p.GetY(), p.GetZ()));
		m_RenderMetadata.ModelMatrix = glm::rotate(m_RenderMetadata.ModelMatrix, angle, glm::vec3(r.GetX(), r.GetY(), r.GetZ()));
		m_RenderMetadata.ModelMatrix = glm::scale(m_RenderMetadata.ModelMatrix, glm::vec3(2));
	}

	glUseProgram(m_Shader->GetRendererID());

	auto& viewMatrix = m_State.CameraController.GetCamera().GetViewProjectionMatrix();
	int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_RenderMetadata.ModelMatrix));

	glActiveTexture(GL_TEXTURE0);
	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Atlas");
	glUniform1i(location, 0);
	glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

	glBindVertexArray(m_RenderMetadata.VertexArray);
	glDrawElements(GL_TRIANGLES, m_RenderMetadata.Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

}

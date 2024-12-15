#pragma once

#include <vector>
#include <GLCore.hpp>
#include <GLCoreUtils.hpp>
#include "../Ecs/Ecs.hpp"
#include "../Terrain/Position2D.hpp"

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

namespace VoxelEngine
{

struct Material
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float_t Shininess;
};

struct PointLight
{
	glm::vec3 Position;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float_t Constant;
	float_t Linear;
	float_t Quadratic;

	PointLight(glm::vec3 pos, glm::vec3 color)
	{
		Position = pos;
		Ambient = color;
		Diffuse = color;
		Specular = color;
		Constant = 0.005f;
		Linear = 0.005f;
		Quadratic = 0.012f;
	}

	PointLight(glm::vec3 pos, glm::vec3 a, glm::vec3 d, glm::vec3 s, float_t x, float_t y, float_t z)
	{
		Position = pos;
		Ambient = a;
		Diffuse = d;
		Specular = s;
		Constant = x;
		Linear = y;
		Quadratic = z;
	}
};

struct DirectionalLight
{
	glm::vec3 Direction;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

struct SpotLight
{
	glm::vec3 Direction;
	glm::vec3 Position;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float_t CutOff;
	float_t OuterCutOff;
	float_t Constant;
	float_t Linear;
	float_t Quadratic;

	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 dif, glm::vec3 spec)
	{
		Position = pos;
		Direction = dir;
		Ambient = glm::vec3(0.0f);
		Diffuse = dif;
		Specular = spec;
		CutOff = glm::cos(glm::radians(2.0f));
		OuterCutOff = glm::cos(glm::radians(3.0f));
		Constant = 0;
		Linear = 0.009f;
		Quadratic = 0;
	}
};

class Renderer
{
public:
	static void Init(GLCore::Window& window);
	static void Shutdown();
	static Renderer& Instance();

	Renderer(GLCore::Window& window);

	void SetDirectionalLight(DirectionalLight light);
	void RenderScene(GLCore::Utils::PerspectiveCamera& camera);

private:
	void Render(GLCore::Utils::PerspectiveCamera& camera, GLCore::Utils::Shader* terrainShader, GLCore::Utils::Shader* meshShader);
	void RenderPass(GLCore::Utils::PerspectiveCamera& camera);

	void RenderMesh(MeshComponent& meshComponent, GLCore::Utils::PerspectiveCamera& camera, glm::mat4& model, GLCore::Utils::Shader* shader);
	void RenderTerrain(std::unordered_map<Position2D, ChunkRenderData>& renderDataMap, GLCore::Utils::PerspectiveCamera& camera, GLCore::Utils::Shader* shader);

	void SetPointLightUniform(GLCore::Utils::Shader& shader, const std::string&, PointLight&);
	void SetDirectionalLightUniform(GLCore::Utils::Shader& shader, const std::string&, DirectionalLight&);
	void SetSpotLightUniform(GLCore::Utils::Shader& shader, const std::string&, SpotLight&);
private:
	GLCore::Window& m_Window;
	VoxelEngine::Texture m_TextureAtlas;
	DirectionalLight m_DirectionalLight;

	GLCore::Utils::Shader* m_TerrainShader;
	GLCore::Utils::Shader* m_MeshShader;
};

inline Renderer* g_Renderer = nullptr;

}
#pragma once

#include <vector>
#include <GLCore.hpp>
#include <GLCoreUtils.hpp>
#include "../Ecs/Ecs.hpp"

namespace VoxelEngine
{

struct Material
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
};

struct PointLight
{
	glm::vec3 Position;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	float Constant;
	float Linear;
	float Quadratic;

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

	PointLight(glm::vec3 pos, glm::vec3 a, glm::vec3 d, glm::vec3 s, float x, float y, float z)
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
	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;

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
	static void Init();
	static void Shutdown();
	static Renderer& Instance();

	void Render(MeshComponent& meshComponent, GLCore::Utils::PerspectiveCamera& camera, glm::mat4 model) const;
	void SetPointLight(GLCore::Utils::Shader& shader, const std::string&, PointLight&) const;
	void SetDirectionalLight(GLCore::Utils::Shader& shader, const std::string&, DirectionalLight&) const;
	void SetSpotLight(GLCore::Utils::Shader& shader, const std::string&, SpotLight&) const;
};

inline Renderer* g_Renderer = nullptr;

}
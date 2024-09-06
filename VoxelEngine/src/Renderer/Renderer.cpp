#include "Renderer.hpp"
#include "../Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

void Renderer::Init()
{
	g_Renderer = new Renderer();
}

void Renderer::Shutdown()
{
	delete g_Renderer;
	g_Renderer = nullptr;
}

Renderer& Renderer::Instance()
{
	return *g_Renderer;
}

void Renderer::Render(MeshComponent& meshComponent, PerspectiveCamera& camera, glm::mat4& model) const
{
	DirectionalLight light = { glm::normalize(glm::vec3(1.0f, -2.0f, 1.0f)), glm::vec3(0.1f), glm::vec3(0.7f), glm::vec3(0.3f) };
	auto shader = meshComponent.GetShader();
	glUseProgram(shader->GetRendererID());
	shader->SetVec3("u_CameraPos", camera.GetPosition());
	shader->SetViewProjection(camera.GetViewProjectionMatrix());
	shader->SetModel(model);
	SetDirectionalLight(*shader, "u_DirectionalLight", light);
	for (Mesh& mesh : meshComponent.GetMeshes())
	{
		uint32_t diffuseNr = 1;
		uint32_t specularNr = 1;
		uint32_t normalNr = 1;
		auto& textures = mesh.GetTextures();
		for (size_t i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == "Diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "Specular")
				number = std::to_string(specularNr++);
			else if (name == "Normal")
				number = std::to_string(normalNr++);
			const std::string uniform = "u_Texture" + name + "_" + number;
			glUniform1i(glGetUniformLocation(shader->GetRendererID(), uniform.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(mesh.GetVAO());
		glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void Renderer::SetPointLight(Shader& shader, const std::string& uniform, PointLight& light) const
{
	shader.SetVec3(uniform + ".Position", light.Position);
	shader.SetVec3(uniform + ".Ambient", light.Ambient);
	shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
	shader.SetVec3(uniform + ".Specular", light.Specular);
	shader.SetFloat(uniform + ".Constant", light.Constant);
	shader.SetFloat(uniform + ".Linear", light.Linear);
	shader.SetFloat(uniform + ".Quadratic", light.Quadratic);
}

void Renderer::SetDirectionalLight(Shader& shader, const std::string& uniform, DirectionalLight& light) const
{
	shader.SetVec3(uniform + ".Direction", light.Direction);
	shader.SetVec3(uniform + ".Ambient", light.Ambient);
	shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
	shader.SetVec3(uniform + ".Specular", light.Specular);
}

void Renderer::SetSpotLight(Shader& shader, const std::string& uniform, SpotLight& light) const
{
	shader.SetVec3(uniform + ".Direction", light.Direction);
	shader.SetVec3(uniform + ".Position", light.Position);
	shader.SetVec3(uniform + ".Ambient", light.Ambient);
	shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
	shader.SetVec3(uniform + ".Specular", light.Specular);
	shader.SetFloat(uniform + ".CutOff", light.CutOff);
	shader.SetFloat(uniform + ".OuterCutOff", light.OuterCutOff);
	shader.SetFloat(uniform + ".Constant", light.Constant);
	shader.SetFloat(uniform + ".Linear", light.Linear);
	shader.SetFloat(uniform + ".Quadratic", light.Quadratic);
}

}

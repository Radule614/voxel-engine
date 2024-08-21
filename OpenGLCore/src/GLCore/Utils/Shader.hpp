#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GLCore::Utils
{
class Shader
{
public:
	~Shader();
	GLuint GetRendererID() const
	{
		return m_RendererID;
	}
	static Shader* FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

	void SetVec3(const std::string&, const glm::vec3&) const;
	void SetVec3(const std::string&, float, float, float) const;
	void SetFloat(const std::string&, float) const;
	void SetInt(const std::string&, int) const;
	void SetViewProjection(const glm::mat4) const;
	void SetModel(const glm::mat4) const;

private:
	Shader() = default;
	void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	GLuint CompileShader(GLenum type, const std::string& source);

private:
	GLuint m_RendererID;
};
}

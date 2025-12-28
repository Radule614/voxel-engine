#include "glpch.hpp"
#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace GLCore::Utils
{

Shader::Shader(GLenum program) : m_RendererID(program)
{
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::SetVec3(const std::string& uniform, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_RendererID, uniform.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string& uniform, const float_t x, const float_t y, const float_t z) const
{
    SetVec3(uniform, glm::vec3(x, y, z));
}

void Shader::SetVec4(const std::string& uniform, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_RendererID, uniform.c_str()), 1, &value[0]);
}

void Shader::SetFloat(const std::string& uniform, const float_t value) const
{
    glUniform1f(glGetUniformLocation(m_RendererID, uniform.c_str()), value);
}

void Shader::SetInt(const std::string& uniform, const int32_t value) const
{
    glUniform1i(glGetUniformLocation(m_RendererID, uniform.c_str()), value);
}

void Shader::SetInt(const std::string& uniform, const int32_t value, int32_t index) const
{
    SetInt(std::format("{}[{}]", uniform, index), value);
}

void Shader::SetBool(const std::string& uniform, const bool b) const
{
    glUniform1i(glGetUniformLocation(m_RendererID, uniform.c_str()), b);
}

void Shader::SetMat4(const std::string& uniform, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_RendererID, uniform.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetIntArray(const std::string& uniform, const std::vector<int32_t>& vector)
{
    glUniform1iv(glGetUniformLocation(m_RendererID, uniform.c_str()), vector.size(), vector.data());
}

void Shader::SetMat4(const std::string& uniform, const glm::mat4& value, int32_t index) const
{
    SetMat4(std::format("{}[{}]", uniform, index), value);
}

void Shader::SetViewProjection(const glm::mat4& value) const { SetMat4("u_ViewProjection", value); }

void Shader::SetModel(const glm::mat4& value) const { SetMat4("u_Model", value); }

}

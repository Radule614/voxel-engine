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

void Shader::Use() const
{
    glUseProgram(m_RendererID);
}

void Shader::SetViewProjection(const glm::mat4& value) const { Set<glm::mat4>("u_ViewProjection", value); }

void Shader::SetModel(const glm::mat4& value) const { Set<glm::mat4>("u_Model", value); }

GLint Shader::GetLocation(const std::string& uniform) const
{
    return glGetUniformLocation(m_RendererID, uniform.c_str());
}

GLint Shader::GetLocationAtIndex(const std::string& uniform, int32_t index) const
{
    return glGetUniformLocation(m_RendererID, std::format("{}[{}]", uniform, index).c_str());
}

// Common types

template<>
void Shader::Set<bool>(const std::string& uniform, const bool& value) const { Set<int32_t>(uniform, value); }

template<>
void Shader::Set<float_t>(const std::string& uniform, const float_t& value) const
{
    glUniform1f(GetLocation(uniform), value);
}

template<>
void Shader::Set<int32_t>(const std::string& uniform, const int32_t& value) const
{
    glUniform1i(GetLocation(uniform), value);
}

template<>
void Shader::Set<int32_t>(const std::string& uniform, const int32_t& value, const int32_t index) const
{
    glUniform1i(GetLocationAtIndex(uniform, index), value);
}

template<>
void Shader::Set<std::vector<int32_t> >(const std::string& uniform, const std::vector<int32_t>& value) const
{
    glUniform1iv(GetLocation(uniform), value.size(), value.data());
}

// Glm

template<>
void Shader::Set<glm::vec3>(const std::string& uniform, const glm::vec3& value) const
{
    glUniform3fv(GetLocation(uniform), 1, &value[0]);
}

template<>
void Shader::Set<glm::vec4>(const std::string& uniform, const glm::vec4& value) const
{
    glUniform4fv(GetLocation(uniform), 1, &value[0]);
}

template<>
void Shader::Set<glm::mat4>(const std::string& uniform, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetLocation(uniform), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::Set<glm::mat4>(const std::string& uniform, const glm::mat4& value, const int32_t index) const
{
    glUniformMatrix4fv(GetLocationAtIndex(uniform, index), 1, GL_FALSE, glm::value_ptr(value));
}

}

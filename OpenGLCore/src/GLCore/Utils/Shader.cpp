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

void Shader::SetVec3(const std::string& uniform, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(m_RendererID, uniform.c_str()), 1, &v[0]);
}

void Shader::SetVec3(const std::string& uniform, float_t x, float_t y, float_t z) const
{
    SetVec3(uniform, glm::vec3(x, y, z));
}

void Shader::SetVec4(const std::string& uniform, const glm::vec4& v) const
{
    glUniform4fv(glGetUniformLocation(m_RendererID, uniform.c_str()), 1, &v[0]);
}

void Shader::SetFloat(const std::string& uniform, const float_t x) const
{
    glUniform1f(glGetUniformLocation(m_RendererID, uniform.c_str()), x);
}

void Shader::SetInt(const std::string& uniform, const int32_t i) const
{
    glUniform1i(glGetUniformLocation(m_RendererID, uniform.c_str()), i);
}

void Shader::SetBool(const std::string& uniform, const bool b) const
{
    glUniform1i(glGetUniformLocation(m_RendererID, uniform.c_str()), b);
}

void Shader::SetViewProjection(const glm::mat4& viewProjection) const
{
    int32_t location = glGetUniformLocation(m_RendererID, "u_ViewProjection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewProjection));
}

void Shader::SetModel(const glm::mat4& model) const
{
    int32_t location = glGetUniformLocation(m_RendererID, "u_Model");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
}

}

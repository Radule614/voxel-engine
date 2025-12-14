#include "glpch.hpp"
#include "Shader.hpp"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace GLCore::Utils
{
static std::string ReadFileAsString(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else { LOG_ERROR("Could not open file '{0}'", filepath); }

    return result;
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

GLuint Shader::CompileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const GLchar* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, 0);
    glCompileShader(shader);
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        glDeleteShader(shader);
        LOG_ERROR("{0}", infoLog.data());
    }
    return shader;
}

Shader* Shader::FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    auto shader = new Shader();
    shader->LoadFromGLSLTextFiles(vertexShaderPath, fragmentShaderPath);
    return shader;
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

void Shader::LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string vertexSource = ReadFileAsString(vertexShaderPath);
    std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

    GLuint program = glCreateProgram();
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    glAttachShader(program, vertexShader);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*) &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        LOG_ERROR("{0}", infoLog.data());
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_RendererID = program;
}
}

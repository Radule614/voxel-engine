//
// Created by RadU on 12/25/2025.
//

#include "glpch.hpp"
#include "ShaderBuilder.hpp"

#include <ranges>

namespace VoxelEngine
{

static GLuint CompileShader(GLenum type, const std::string& source);
static std::string ReadFileAsString(const std::string& filepath);

ShaderBuilder::ShaderBuilder() : m_Program(0)
{
}

ShaderBuilder& ShaderBuilder::AddShader(GLenum type, const std::string& shaderPath)
{
    const std::string shaderSource = ReadFileAsString(shaderPath);
    const GLuint shader = CompileShader(type, shaderSource);

    if (m_Program == 0)
        m_Program = glCreateProgram();

    glAttachShader(m_Program, shader);

    m_Shaders.emplace_back(type, shader);

    return *this;
}

GLCore::Utils::Shader* ShaderBuilder::Build()
{
    glLinkProgram(m_Program);

    GLint isLinked = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, (int*) &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(m_Program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(m_Program);

        for (const auto& shader: m_Shaders | std::views::values)
            glDeleteShader(shader);

        LOG_ERROR("{0}", infoLog.data());

        return nullptr;
    }

    for (const auto& shader: m_Shaders | std::views::values)
    {
        glDetachShader(m_Program, shader);
        glDeleteShader(shader);
    }

    const auto shader = new GLCore::Utils::Shader(m_Program);

    m_Program = 0;

    return shader;
}

static GLuint CompileShader(GLenum type, const std::string& source)
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

}

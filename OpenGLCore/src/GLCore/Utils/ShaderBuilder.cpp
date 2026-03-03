//
// Created by RadU on 12/25/2025.
//

#include "glpch.hpp"
#include "ShaderBuilder.hpp"

#include <ranges>
#include <filesystem>

#ifndef ROOT_SHADER_PATH
#error "ROOT_SHADER_PATH is not defined"
#endif

namespace VoxelEngine
{

static std::string ReadFileAsString(const std::string& filepath);
static std::string ResolveIncludes(const std::string& source);
static GLuint CompileShader(GLenum type, const std::string& source);

ShaderBuilder::ShaderBuilder() : m_Program(0)
{
}

ShaderBuilder& ShaderBuilder::AddShader(GLenum type, const std::string& shaderPath)
{
    const std::string shaderSource = ResolveIncludes(ReadFileAsString(shaderPath));
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

static std::string ResolveIncludes(const std::string& source)
{
    static std::unordered_map<std::string, std::string> LoadedIncludes{};

    std::stringstream result;
    std::istringstream input(source);
    std::string line;

    while (std::getline(input, line))
    {
        if (line.find("#include") != std::string::npos)
        {
            const size_t firstQuote = line.find('"');
            const size_t lastQuote = line.find_last_of('"');

            if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote != lastQuote)
            {
                std::string includeName = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);

                if (LoadedIncludes.contains(includeName))
                {
                    result << LoadedIncludes[includeName];

                    continue;
                }

                std::string includeSource = ReadFileAsString(includeName);
                LoadedIncludes[includeName] = includeSource;
                result << includeSource;

                continue;
            }
        }

        result << line << "\n";
    }

    return result.str();
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
    std::filesystem::path fullPath = std::filesystem::current_path() / ROOT_SHADER_PATH / filepath;

    std::string result;
    std::ifstream in(fullPath, std::ios::in | std::ios::binary);

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

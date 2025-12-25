//
// Created by RadU on 12/25/2025.
//

#pragma once

#include <string>
#include <vector>

#include "Shader.hpp"
#include "glad/glad.h"

namespace VoxelEngine
{

class ShaderBuilder
{
public:
    ShaderBuilder();
    ~ShaderBuilder() = default;

    ShaderBuilder& AddShader(GLenum type, const std::string& shaderPath);
    GLCore::Utils::Shader* Build();

private:
    GLuint m_Program;
    std::vector<std::pair<GLenum, GLuint> > m_Shaders;
};

}

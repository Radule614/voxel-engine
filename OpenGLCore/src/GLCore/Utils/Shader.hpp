#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GLCore::Utils
{

class Shader
{
public:
    explicit Shader(GLenum program);
    ~Shader();

    GLuint GetRendererID() const { return m_RendererID; }

    void Use() const;

    void SetModel(const glm::mat4& value) const;
    void SetViewProjection(const glm::mat4& value) const;

    template<typename T>
    void Set(const std::string& uniform, const T& value) const;

    template<typename T>
    void Set(const std::string& uniform, const T& value, int32_t index) const;

private:
    GLint GetLocation(const std::string& uniform) const;
    GLint GetLocationAtIndex(const std::string& uniform, int32_t index) const;

private:
    GLuint m_RendererID;
};

// Common types

template<>
void Shader::Set<bool>(const std::string& uniform, const bool& value) const;

template<>
void Shader::Set<float_t>(const std::string& uniform, const float_t& value) const;

template<>
void Shader::Set<int32_t>(const std::string& uniform, const int32_t& value) const;

template<>
void Shader::Set<int32_t>(const std::string& uniform, const int32_t& value, int32_t index) const;

template<>
void Shader::Set<std::vector<int32_t> >(const std::string& uniform, const std::vector<int32_t>& value) const;

// Glm

template<>
void Shader::Set<glm::vec3>(const std::string& uniform, const glm::vec3& value) const;

template<>
void Shader::Set<glm::vec4>(const std::string& uniform, const glm::vec4& value) const;

template<>
void Shader::Set<glm::mat4>(const std::string& uniform, const glm::mat4& value) const;

template<>
void Shader::Set<glm::mat4>(const std::string& uniform, const glm::mat4& value, int32_t index) const;

}

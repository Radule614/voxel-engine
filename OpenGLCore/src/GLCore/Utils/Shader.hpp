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
    
    void SetVec3(const std::string&, const glm::vec3&) const;
    void SetVec3(const std::string&, float_t, float_t, float_t) const;
    void SetVec4(const std::string&, const glm::vec4&) const;
    void SetViewProjection(const glm::mat4&) const;
    void SetModel(const glm::mat4&) const;
    void SetFloat(const std::string&, float_t) const;
    void SetInt(const std::string&, int32_t) const;
    void SetBool(const std::string&, bool) const;

private:
    GLuint m_RendererID;
};

}

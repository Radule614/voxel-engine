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

    void SetVec3(const std::string& uniform, const glm::vec3& value) const;
    void SetVec3(const std::string& uniform, float_t x, float_t y, float_t z) const;
    void SetVec4(const std::string& uniform, const glm::vec4& value) const;
    void SetViewProjection(const glm::mat4& value) const;
    void SetModel(const glm::mat4& value) const;
    void SetFloat(const std::string& uniform, float_t value) const;
    void SetInt(const std::string& uniform, int32_t value) const;
    void SetInt(const std::string& uniform, int32_t value, int32_t index) const;
    void SetBool(const std::string& uniform, bool value) const;

    void SetMat4(const std::string& uniform, const glm::mat4& value, int32_t index) const;
    void SetMat4(const std::string& uniform, const glm::mat4& value) const;

    void SetIntArray(const std::string& uniform, const std::vector<int32_t>& vector);

private:
    GLuint m_RendererID;
};

}

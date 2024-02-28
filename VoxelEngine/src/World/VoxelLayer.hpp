#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"

class VoxelLayer : public GLCore::Layer
{
public:
    VoxelLayer();
    ~VoxelLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(GLCore::Event &event) override;
    virtual void OnUpdate(GLCore::Timestep ts) override;
    virtual void OnImGuiRender() override;

private:
    GLCore::Utils::OrthographicCameraController m_CameraController;
    GLCore::Utils::Shader *m_Shader;

    GLuint m_VA, m_VB, m_IB, m_InstanceVB;
    std::vector<uint32_t> m_Indices;
    glm::vec4 m_Color = {0.8f, 0.2f, 0.3f, 1.0f};
};

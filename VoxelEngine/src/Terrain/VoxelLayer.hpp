#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"

#include "Chunk.hpp"

namespace Terrain
{
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
    GLCore::Utils::PerspectiveCameraController m_CameraController;
    GLCore::Utils::Shader *m_Shader;
    GLuint m_VA, m_VB, m_IB;
    std::vector<uint32_t> m_Indices;

    Chunk m_Chunk;

    //TEMP
    GLCore::Utils::TextureManager m_TextureManager;
    GLCore::Utils::Texture m_TextureAtlas;
};
}; // namespace Terrain

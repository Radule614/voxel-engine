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
};

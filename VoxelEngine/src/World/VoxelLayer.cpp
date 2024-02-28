#include "VoxelLayer.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

VoxelLayer::VoxelLayer() : m_CameraController(16.0f / 9.0f)
{
}

VoxelLayer::~VoxelLayer()
{
}

void VoxelLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void VoxelLayer::OnDetach()
{
}


void VoxelLayer::OnEvent(GLCore::Event &event)
{
    m_CameraController.OnEvent(event);
}

void VoxelLayer::OnUpdate(Timestep ts)
{
    m_CameraController.OnUpdate(ts);
}

void VoxelLayer::OnImGuiRender()
{
}

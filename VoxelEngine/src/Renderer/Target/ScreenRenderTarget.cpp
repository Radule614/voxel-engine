//
// Created by RadU on 2/8/2026.
//

#include "ScreenRenderTarget.hpp"

#include "glad/glad.h"

namespace VoxelEngine
{

ScreenRenderTarget::ScreenRenderTarget(const int32_t width, const int32_t height) : m_Width(width), m_Height(height)
{
}

void ScreenRenderTarget::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int ScreenRenderTarget::GetWidth() const { return m_Width; }

int ScreenRenderTarget::GetHeight() const { return m_Height; }

}

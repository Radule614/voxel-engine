//
// Created by RadU on 2/8/2026.
//

#include "TextureRenderTarget.hpp"
#include "GLCoreUtils.hpp"

namespace VoxelEngine
{

TextureRenderTarget::TextureRenderTarget(const int32_t width, const int32_t height) : m_Width(width), m_Height(height)
{
    glGenFramebuffers(1, &m_FrameBufferObject);
    glGenRenderbuffers(1, &m_RenderBufferObject);
    glGenTextures(1, &m_Texture);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);

    // Color texture
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferObject);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureRenderTarget::~TextureRenderTarget()
{
    glDeleteRenderbuffers(1, &m_RenderBufferObject);
    glDeleteFramebuffers(1, &m_FrameBufferObject);
    glDeleteTextures(1, &m_Texture);
}

void TextureRenderTarget::Bind() const
{
    glViewport(0, 0, m_Width, m_Height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TextureRenderTarget::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int TextureRenderTarget::GetWidth() const { return m_Width; }

int TextureRenderTarget::GetHeight() const { return m_Height; }

void TextureRenderTarget::Resize(const int32_t width, const int32_t height)
{
    m_Width = std::max(width, 0);
    m_Height = std::max(height, 0);

    // Resize color texture
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Resize depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

GLuint TextureRenderTarget::GetTexture() const { return m_Texture; }

}

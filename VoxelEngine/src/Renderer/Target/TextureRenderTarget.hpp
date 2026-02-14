//
// Created by RadU on 2/8/2026.
//

#pragma once

#include <cstdint>
#include "RenderTarget.hpp"
#include "glad/glad.h"

namespace VoxelEngine
{

class TextureRenderTarget : public RenderTarget
{
public:
    TextureRenderTarget(int32_t width, int32_t height);
    ~TextureRenderTarget() override;
    TextureRenderTarget(const TextureRenderTarget&) = delete;
    TextureRenderTarget& operator=(const TextureRenderTarget&) = delete;

    void Bind() const override;
    void Unbind() const override;

    int GetWidth() const override;
    int GetHeight() const override;

    void Resize(int32_t width, int32_t height);

    GLuint GetTexture() const;

private:
    int32_t m_Width;
    int32_t m_Height;

    GLuint m_FrameBufferObject = 0;
    GLuint m_RenderBufferObject = 0;

    GLuint m_Texture = 0;

};

}

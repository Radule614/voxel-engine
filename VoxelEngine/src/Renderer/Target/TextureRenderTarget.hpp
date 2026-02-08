//
// Created by RadU on 2/8/2026.
//

#pragma once

#include "RenderTarget.hpp"
#include "glad/glad.h"

namespace VoxelEngine
{

class TextureRenderTarget : public RenderTarget
{
public:
    TextureRenderTarget();

    void Bind() const override;
    int GetWidth() const override;
    int GetHeight() const override;

private:
    GLuint m_Fbo;

};

}

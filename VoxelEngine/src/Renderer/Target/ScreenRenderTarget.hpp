//
// Created by RadU on 2/8/2026.
//

#pragma once

#include <cstdint>

#include "RenderTarget.hpp"

namespace VoxelEngine
{

class ScreenRenderTarget : public RenderTarget
{
public:
    ScreenRenderTarget(int32_t width, int32_t height);

    void Bind() const override;
    int GetWidth() const override;
    int GetHeight() const override;

private:
    int32_t m_Width;
    int32_t m_Height;
};

}

//
// Created by RadU on 2/8/2026.
//

#pragma once

namespace VoxelEngine
{

class RenderTarget
{
public:
    virtual ~RenderTarget() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
};

}

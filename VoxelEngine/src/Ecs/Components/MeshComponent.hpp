#pragma once

#include "../../Assets/Gltf/Model.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    std::string Name;
    const std::vector<RenderPrimitive> Primitives;

    explicit MeshComponent(const std::vector<RenderPrimitive>& primitives) : Primitives(primitives)
    {
    }
};

}

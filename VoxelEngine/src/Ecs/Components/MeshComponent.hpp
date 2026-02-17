#pragma once

#include "../ComponentGui.hpp"
#include "../../Assets/Gltf/Model.hpp"

namespace VoxelEngine
{

struct MeshComponent : ComponentGui
{
    std::string Name;
    const std::vector<RenderPrimitive> Primitives;

    explicit MeshComponent(std::string name, const std::vector<RenderPrimitive>& primitives);
    ~MeshComponent() override = default;

    std::string GetName() override;
    void DrawGui() override;
};

}

#pragma once

#include "../../Assets/Gltf/Model.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    explicit MeshComponent(const Model& model) : Model(model)
    {
    }

    const Model& Model;
};

}

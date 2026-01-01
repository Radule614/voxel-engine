#pragma once

#include "../../Assets/Gltf/Model.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    const Model& Model;

    explicit MeshComponent(const VoxelEngine::Model& model) : Model(model)
    {
    }
};

}

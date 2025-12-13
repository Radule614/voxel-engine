#pragma once

#include "../../Assets/Model.hpp"

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

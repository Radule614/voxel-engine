#pragma once

#include "../../Assets/Model.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    explicit MeshComponent(const std::shared_ptr<Model>& model) : Model(model)
    {
    }

    std::shared_ptr<Model> Model;
};

}

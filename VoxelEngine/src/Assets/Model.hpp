#pragma once
#include "Mesh.hpp"

namespace VoxelEngine
{

struct Model
{
    std::vector<Mesh> Meshes;

    Model()
    {
    }

    Model(const std::vector<Mesh>& meshes) : Meshes(meshes)
    {
    }
};

}

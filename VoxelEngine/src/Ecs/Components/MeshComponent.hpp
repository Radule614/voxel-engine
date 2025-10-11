#pragma once

#include <GLCoreUtils.hpp>
#include "../../Assets/Mesh.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    explicit MeshComponent(const std::vector<Mesh>& meshes)
        : Meshes(meshes)
    {
    }

    std::shared_ptr<GLCore::Utils::Shader> Shader;
    std::vector<Mesh> Meshes;
};

}

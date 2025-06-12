#pragma once

#include <GLCoreUtils.hpp>
#include "../../Assets/Mesh.hpp"

namespace VoxelEngine
{

struct MeshComponent
{
    MeshComponent(const std::shared_ptr<GLCore::Utils::Shader>& shader, const std::vector<Mesh>& meshes)
        : Shader(shader), Meshes(meshes)
    {
    }

    std::shared_ptr<GLCore::Utils::Shader> Shader;
    std::vector<Mesh> Meshes;
};

}

#pragma once

#include <vector>

#include "Voxel.hpp"
#include "VoxelVertex.hpp"

namespace VoxelEngine
{

class VoxelMeshBuilder
{
public:
    VoxelMeshBuilder() = default;
    ~VoxelMeshBuilder() = default;

    std::vector<VoxelVertex> FromVoxel(Voxel& voxel);
    std::vector<VoxelVertex> FromVoxel(Voxel& voxel, VoxelFace face);
    std::vector<VoxelVertex> FromVoxelFaces(Voxel& voxel, bool faces[6]);
    std::vector<VoxelVertex> FromVoxelExceptFaces(Voxel& voxel, bool faces[6]);
};

};

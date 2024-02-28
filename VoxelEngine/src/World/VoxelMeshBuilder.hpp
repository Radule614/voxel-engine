#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Voxel.hpp"

class VoxelMeshBuilder
{
public:
    VoxelMeshBuilder();
    ~VoxelMeshBuilder();

    std::vector<float_t> FromVoxel(Voxel &voxel);

    static void Init();

    static std::vector<glm::vec2> s_TextureCoordinates;
    static std::unordered_map<VoxelFace, std::vector<glm::vec3>> s_PositionMap;
    static std::unordered_map<VoxelFace, glm::vec3> s_NormalMap;

private:
    static bool s_Initialized;
};

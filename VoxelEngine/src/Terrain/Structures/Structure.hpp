#pragma once

#include "../Voxel.hpp"
#include <vector>

namespace VoxelEngine
{

class TreeFactory;

class Structure
{
public:
    explicit Structure(VoxelType rootType, const std::vector<std::pair<glm::i32vec3, VoxelType> >& data);
    ~Structure();

    void SetRootPosition(Position3D position);

    const Voxel& GetRoot() const;
    const std::vector<std::pair<glm::i32vec3, VoxelType> >& GetVoxelData() const;

private:
    Voxel m_Root;
    const std::vector<std::pair<glm::i32vec3, VoxelType> >& m_Data;
};

}

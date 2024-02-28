#include "VoxelMeshBuilder.hpp"

bool VoxelMeshBuilder::s_Initialized = false;
std::vector<glm::vec2> VoxelMeshBuilder::s_TextureCoordinates = {glm::vec2(0.0f, 0.0f),
                                                                 glm::vec2(0.0f, 1.0f),
                                                                 glm::vec2(1.0f, 1.0f),
                                                                 glm::vec2(1.0f, 0.0f)};

std::unordered_map<VoxelFace, std::vector<glm::vec3>> VoxelMeshBuilder::s_PositionMap = {};
std::unordered_map<VoxelFace, glm::vec3> VoxelMeshBuilder::s_NormalMap = {};
void VoxelMeshBuilder::Init()
{
    s_PositionMap.insert({VoxelFace::TOP,
                          {glm::vec3(-1.0f, 1.0f, 1.0f),
                           glm::vec3(-1.0f, 1.0f, -1.0f),
                           glm::vec3(1.0f, 1.0f, -1.0f),
                           glm::vec3(1.0f, 1.0f, 1.0f)}});
    s_PositionMap.insert({VoxelFace::BOTTOM,
                          {glm::vec3(-1.0f, -1.0f, 1.0f),
                           glm::vec3(-1.0f, -1.0f, -1.0f),
                           glm::vec3(1.0f, -1.0f, -1.0f),
                           glm::vec3(1.0f, -1.0f, 1.0f)}});
    s_PositionMap.insert({VoxelFace::FRONT,
                          {glm::vec3(-1.0f, -1.0f, 1.0f),
                           glm::vec3(-1.0f, 1.0f, 1.0f),
                           glm::vec3(1.0f, 1.0f, 1.0f),
                           glm::vec3(1.0f, -1.0f, 1.0f)}});
    s_PositionMap.insert({VoxelFace::RIGHT,
                          {glm::vec3(1.0f, -1.0f, 1.0f),
                           glm::vec3(1.0f, 1.0f, 1.0f),
                           glm::vec3(1.0f, 1.0f, -1.0f),
                           glm::vec3(1.0f, -1.0f, -1.0f)}});
    s_PositionMap.insert({VoxelFace::BACK,
                          {glm::vec3(-1.0f, -1.0f, 1.0f),
                           glm::vec3(-1.0f, 1.0f, -1.0f),
                           glm::vec3(1.0f, 1.0f, -1.0f),
                           glm::vec3(1.0f, -1.0f, -1.0f)}});
    s_PositionMap.insert({VoxelFace::LEFT,
                          {glm::vec3(-1.0f, -1.0f, -1.0f),
                           glm::vec3(-1.0f, 1.0f, -1.0f),
                           glm::vec3(-1.0f, 1.0f, 1.0f),
                           glm::vec3(-1.0f, -1.0f, 1.0f)}});
    s_NormalMap.insert({VoxelFace::TOP, glm::vec3(0.0f, 1.0f, 0.0f)});
    s_NormalMap.insert({VoxelFace::BOTTOM, glm::vec3(0.0f, -1.0f, 0.0f)});
    s_NormalMap.insert({VoxelFace::FRONT, glm::vec3(0.0f, 0.0f, 1.0f)});
    s_NormalMap.insert({VoxelFace::RIGHT, glm::vec3(1.0f, 0.0f, 0.0f)});
    s_NormalMap.insert({VoxelFace::BACK, glm::vec3(0.0f, 0.0f, -1.0f)});
    s_NormalMap.insert({VoxelFace::LEFT, glm::vec3(-1.0f, 0.0f, 0.0f)});
}

VoxelMeshBuilder::VoxelMeshBuilder()
{
    if (!s_Initialized)
    {
        Init();
        s_Initialized = true;
    }
}

VoxelMeshBuilder::~VoxelMeshBuilder()
{
}

std::vector<float_t> VoxelMeshBuilder::FromVoxel(Voxel &voxel)
{
    std::vector<float_t> data = {};
    for (uint16_t i = 0; i < 6; ++i)
    {
        VoxelFace face = static_cast<VoxelFace>(i);
        if (!voxel.IsFaceVisible(face))
            continue;

        std::vector<glm::vec3> positions = s_PositionMap.at(face);
        glm::vec3 normal = s_NormalMap.at(face);

        for (uint16_t p = 0; p < positions.size(); ++p)
        {
            glm::vec3 pos = positions[p];
            glm::vec2 texCoord = s_TextureCoordinates[p];

            data.push_back(pos.x);
            data.push_back(pos.y);
            data.push_back(pos.z);
            data.push_back(normal.x);
            data.push_back(normal.y);
            data.push_back(normal.z);
            data.push_back(texCoord.x);
            data.push_back(texCoord.y);
        }
    }
    return data;
}

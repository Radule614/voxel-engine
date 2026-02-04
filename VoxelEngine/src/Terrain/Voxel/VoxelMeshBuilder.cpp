#include "VoxelMeshBuilder.hpp"
#include <GLCoreUtils.hpp>

#include "../../Config.hpp"

namespace VoxelEngine
{

static const std::vector<glm::vec2> TextureCoordinates = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f}
};

static const std::unordered_map<VoxelFace, std::vector<glm::vec3> > PositionMap = {
    {
        TOP,
        {
            {0.5f, 0.5f, 0.5f},
            {0.5f, 0.5f, -0.5f},
            {-0.5f, 0.5f, -0.5f},
            {-0.5f, 0.5f, 0.5f}
        }
    },
    {
        BOTTOM,
        {
            {-0.5f, -0.5f, 0.5f},
            {-0.5f, -0.5f, -0.5f},
            {0.5f, -0.5f, -0.5f},
            {0.5f, -0.5f, 0.5f}
        }
    },
    {
        FRONT,
        {
            {0.5f, -0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},
            {-0.5f, 0.5f, 0.5f},
            {-0.5f, -0.5f, 0.5f}
        }
    },
    {
        RIGHT,
        {
            {0.5f, -0.5f, -0.5f},
            {0.5f, 0.5f, -0.5f},
            {0.5f, 0.5f, 0.5f},
            {0.5f, -0.5f, 0.5f}
        }
    },
    {
        BACK,
        {
            {-0.5f, -0.5f, -0.5f},
            {-0.5f, 0.5f, -0.5f},
            {0.5f, 0.5f, -0.5f},
            {0.5f, -0.5f, -0.5f}
        }
    },
    {
        LEFT,
        {
            {-0.5f, -0.5f, 0.5f},
            {-0.5f, 0.5f, 0.5f},
            {-0.5f, 0.5f, -0.5f},
            {-0.5f, -0.5f, -0.5f}
        }
    },
};

static const std::unordered_map<VoxelFace, glm::vec3> NormalMap = {
    {TOP, {0.0f, 1.0f, 0.0f}},
    {BOTTOM, {0.0f, -1.0f, 0.0f}},
    {FRONT, {0.0f, 0.0f, 1.0f}},
    {RIGHT, {1.0f, 0.0f, 0.0f}},
    {BACK, {0.0f, 0.0f, -1.0f}},
    {LEFT, {-1.0f, 0.0f, 0.0f}},
};

static const std::unordered_map<VoxelFace, glm::vec4> TangentMap = {
    {TOP, {1, 0, 0, 1}},
    {BOTTOM, {1, 0, 0, 1}},
    {FRONT, {1, 0, 0, 1}},
    {RIGHT, {0, 0, -1, 1}},
    {BACK, {-1, 0, 0, 1}},
    {LEFT, {0, 0, 1, 1}},
};

static const std::unordered_map<VoxelType, std::vector<int32_t> > FaceTextureMap = {
    {STONE, {0, 0, 0, 0, 0, 0}},
    {DIRT, {1, 0, 1, 0, 1, 0}},
    {DIRT_GRASS, {3, 0, 1, 0, 2, 0}},
    {LOG, {5, 0, 5, 0, 4, 0}},
    {LEAVES, {6, 0, 6, 0, 6, 0}},
    {SNOW, {8, 0, 8, 0, 8, 0}},
    {DIRT_SNOW, {8, 0, 1, 0, 7, 0}},
    {ICE, {9, 0, 9, 0, 9, 0}},
    {LAMP, {10, 0, 10, 0, 10, 0}},
    {SAND, {0, 1, 0, 1, 0, 1}},
    {CACTUS, {3, 1, 3, 1, 2, 1}},
    {DARK_LOG, {5, 1, 5, 1, 4, 1}},
    {DARK_LEAVES, {6, 1, 6, 1, 6, 1}},
    {SOUL, {0, 4, 0, 4, 0, 4}},
};

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxel(Voxel& voxel)
{
    bool faces[6] = {true, true, true, true, true, true};

    return FromVoxelFaces(voxel, faces);
}

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxel(Voxel& voxel, VoxelFace face)
{
    bool faces[6] = {false, false, false, false, false, false};
    faces[face] = true;

    return FromVoxelFaces(voxel, faces);
}

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxelExceptFaces(Voxel& voxel, bool faces[6])
{
    for (size_t i = 0; i < 6; ++i)
        faces[i] = !faces[i];

    return FromVoxelFaces(voxel, faces);
}

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxelFaces(Voxel& voxel, bool faces[6])
{
    GLCORE_ASSERT(voxel.GetVoxelType() != VoxelType::AIR, "Voxel type AIR doesn't have texture assigned to it.");
    std::vector<VoxelVertex> data = {};
    const std::vector<int32_t>& texMap = FaceTextureMap.at(voxel.GetVoxelType());
    float_t textureUnit = 1.0f / 16.0f;

    for (size_t i = 0; i < 6; ++i)
    {
        auto face = static_cast<VoxelFace>(i);
        if (!voxel.IsFaceVisible(face) || !faces[face])
            continue;

        const std::vector<glm::vec3>& positions = PositionMap.at(face);
        int32_t texMapX = texMap[4];
        int32_t texMapY = texMap[5];

        if (i == 0)
        {
            texMapX = texMap[0];
            texMapY = texMap[1];
        }
        else if (i == 1)
        {
            texMapX = texMap[2];
            texMapY = texMap[3];
        }

        for (size_t p = 0; p < positions.size(); ++p)
        {
            glm::vec3 pos = positions[p];
            glm::vec2 texCoord = TextureCoordinates[p];

            glm::vec2 atlasTexCoord = glm::vec2(texMapX * textureUnit, 1.0f - (texMapY + 1.0f) * textureUnit);
            if (texCoord.x == 1.0f)
                atlasTexCoord.x += textureUnit;
            if (texCoord.y == 1.0f)
                atlasTexCoord.y += textureUnit;

            auto voxelPosition = voxel.GetPosition();

            VoxelVertex v{};
            v.Position = pos + static_cast<glm::vec3>(voxelPosition);
            v.Normal = NormalMap.at(face);
            v.TexCoords = atlasTexCoord;
            v.Tangent = TangentMap.at(face);

            data.push_back(v);
        }
    }

    return data;
}

}

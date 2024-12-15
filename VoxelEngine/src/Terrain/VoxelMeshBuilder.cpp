#include "VoxelMeshBuilder.hpp"
#include <GLCoreUtils.hpp>

namespace VoxelEngine
{

bool VoxelMeshBuilder::s_Initialized = false;
std::vector<glm::vec2> VoxelMeshBuilder::s_TextureCoordinates = { glm::vec2(0.0f, 0.0f),
																 glm::vec2(0.0f, 1.0f),
																 glm::vec2(1.0f, 1.0f),
																 glm::vec2(1.0f, 0.0f) };

std::unordered_map<VoxelFace, std::vector<glm::vec3>> VoxelMeshBuilder::s_PositionMap = {};
std::unordered_map<VoxelFace, glm::vec3> VoxelMeshBuilder::s_NormalMap = {};
std::unordered_map<VoxelType, std::vector<int32_t>> VoxelMeshBuilder::s_FaceTextureMap = {};

void VoxelMeshBuilder::Init()
{
	s_PositionMap.insert({ VoxelFace::TOP,
						  {glm::vec3(-0.5f, 0.5f, 0.5f),
						   glm::vec3(-0.5f, 0.5f, -0.5f),
						   glm::vec3(0.5f, 0.5f, -0.5f),
						   glm::vec3(0.5f, 0.5f, 0.5f)} });
	s_PositionMap.insert({ VoxelFace::BOTTOM,
						  {glm::vec3(0.5f, -0.5f, 0.5f),
						   glm::vec3(0.5f, -0.5f, -0.5f),
						   glm::vec3(-0.5f, -0.5f, -0.5f),
						   glm::vec3(-0.5f, -0.5f, 0.5f)} });
	s_PositionMap.insert({ VoxelFace::FRONT,
						  {glm::vec3(-0.5f, -0.5f, 0.5f),
						   glm::vec3(-0.5f, 0.5f, 0.5f),
						   glm::vec3(0.5f, 0.5f, 0.5f),
						   glm::vec3(0.5f, -0.5f, 0.5f)} });
	s_PositionMap.insert({ VoxelFace::RIGHT,
						  {glm::vec3(0.5f, -0.5f, 0.5f),
						   glm::vec3(0.5f, 0.5f, 0.5f),
						   glm::vec3(0.5f, 0.5f, -0.5f),
						   glm::vec3(0.5f, -0.5f, -0.5f)} });
	s_PositionMap.insert({ VoxelFace::BACK,
						  {glm::vec3(0.5f, -0.5f, -0.5f),
						   glm::vec3(0.5f, 0.5f, -0.5f),
						   glm::vec3(-0.5f, 0.5f, -0.5f),
						   glm::vec3(-0.5f, -0.5f, -0.5f)} });
	s_PositionMap.insert({ VoxelFace::LEFT,
						  {glm::vec3(-0.5f, -0.5f, -0.5f),
						   glm::vec3(-0.5f, 0.5f, -0.5f),
						   glm::vec3(-0.5f, 0.5f, 0.5f),
						   glm::vec3(-0.5f, -0.5f, 0.5f)} });
	s_NormalMap.insert({ VoxelFace::TOP, glm::vec3(0.0f, 1.0f, 0.0f) });
	s_NormalMap.insert({ VoxelFace::BOTTOM, glm::vec3(0.0f, -1.0f, 0.0f) });
	s_NormalMap.insert({ VoxelFace::FRONT, glm::vec3(0.0f, 0.0f, 1.0f) });
	s_NormalMap.insert({ VoxelFace::RIGHT, glm::vec3(1.0f, 0.0f, 0.0f) });
	s_NormalMap.insert({ VoxelFace::BACK, glm::vec3(0.0f, 0.0f, -1.0f) });
	s_NormalMap.insert({ VoxelFace::LEFT, glm::vec3(-1.0f, 0.0f, 0.0f) });

	//TOP BOTTOM LEFT
	s_FaceTextureMap.insert({ VoxelType::STONE, {0, 0, 0, 0, 0, 0} });
	s_FaceTextureMap.insert({ VoxelType::DIRT, {1, 0, 1, 0, 1, 0} });
	s_FaceTextureMap.insert({ VoxelType::GRASS, {3, 0, 1, 0, 2, 0} });
	s_FaceTextureMap.insert({ VoxelType::LOG, {5, 0, 5, 0, 4, 0} });
	s_FaceTextureMap.insert({ VoxelType::LEAVES, {6, 0, 6, 0, 6, 0} });
	s_FaceTextureMap.insert({ VoxelType::SNOW, {8, 0, 8, 0, 8, 0} });
	s_FaceTextureMap.insert({ VoxelType::DIRT_SNOW, {8, 0, 1, 0, 7, 0} });
	s_FaceTextureMap.insert({ VoxelType::LAMP , {9, 0, 9, 0, 9, 0}});
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

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxel(Voxel& voxel)
{
	bool faces[6] = { true, true, true, true, true, true };
	return FromVoxelFaces(voxel, faces);
}

std::vector<VoxelVertex> VoxelMeshBuilder::FromVoxel(Voxel& voxel, VoxelFace f)
{
	bool faces[6] = { false, false, false, false, false, false };
	faces[f] = true;
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
	std::vector<int32_t>& texMap = s_FaceTextureMap.at(voxel.GetVoxelType());
	float_t textureUnit = 1.0f / 16.0f;
	for (size_t i = 0; i < 6; ++i)
	{
		VoxelFace face = static_cast<VoxelFace>(i);
		if (!voxel.IsFaceVisible(face) || !faces[face])
			continue;

		std::vector<glm::vec3> positions = s_PositionMap.at(face);
		glm::vec3 normal = s_NormalMap.at(face);
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
			glm::vec2 texCoord = s_TextureCoordinates[p];

			glm::vec2 atlasTexCoord = glm::vec2(texMapX * textureUnit, 1.0f - (texMapY + 1.0f) * textureUnit);
			if (texCoord.x == 1.0f)
				atlasTexCoord.x += textureUnit;
			if (texCoord.y == 1.0f)
				atlasTexCoord.y += textureUnit;

			VoxelVertex v;
			v.Position = pos + (glm::vec3)(glm::i16vec3)voxel.GetPosition();
			v.Normal = normal;
			v.TexCoords = atlasTexCoord;
			v.Light = voxel.GetLight();
			data.push_back(v);
		}
	}
	return data;
}

};

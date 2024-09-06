#pragma once
#include "Mesh.hpp"

namespace VoxelEngine
{

struct Model
{
	std::vector<Mesh> Meshes;
	Model() {}
	Model(std::vector<Mesh> meshes) : Meshes(meshes) {}
};

}
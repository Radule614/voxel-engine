#pragma once

#include <GLCoreUtils.hpp>
#include "../Assets/Mesh.hpp"

namespace VoxelEngine
{

class MeshComponent
{
public:
	MeshComponent(std::shared_ptr<GLCore::Utils::Shader> shader, std::vector<Mesh> meshes) : m_Shader(shader), m_Meshes(meshes)
	{
	}

	inline std::vector<Mesh>& GetMeshes()
	{
		return m_Meshes;
	}

private:
	std::shared_ptr<GLCore::Utils::Shader> m_Shader;
	std::vector<Mesh> m_Meshes;
};

}
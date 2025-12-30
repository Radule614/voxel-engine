#pragma once

#include <GLCore.hpp>
#include <GLCoreUtils.hpp>

#include "PointLight.hpp"
#include "../Ecs/Components/TerrainComponent.hpp"
#include "../Ecs/Components/MeshComponent.hpp"

namespace VoxelEngine
{

class Renderer
{
public:
    explicit Renderer(GLCore::Window& window);
    ~Renderer();

    void RenderScene(const GLCore::Utils::PerspectiveCamera& camera) const;
    std::vector<PointLight>& GetPointLights();

private:
    void Render(const GLCore::Utils::Shader& shader) const;

    void DepthPass() const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void RenderTerrain(const GLCore::Utils::Shader& shader, const TerrainComponent& terrainComponent) const;
    void RenderLights() const;

private:
    GLCore::Window& m_Window;

    std::vector<PointLight> m_PointLights;

    GLuint m_DepthMapFbo;

    GLCore::Utils::Shader* m_PbrShader;
    GLCore::Utils::Shader* m_DepthShader;
    GLCore::Utils::Shader* m_SimpleShader;
};

}

namespace GLCore::Utils
{

template<>
void Shader::Set<VoxelEngine::Material>(const std::string& uniform, const VoxelEngine::Material& value) const;

}

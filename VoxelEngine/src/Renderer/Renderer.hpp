#pragma once

#include <GLCore.hpp>
#include <GLCoreUtils.hpp>
#include "entt.hpp"
#include "PointLight.hpp"
#include "../Ecs/Components/LightComponent.hpp"
#include "../Ecs/Components/TerrainMeshComponent.hpp"
#include "../Ecs/Components/MeshComponent.hpp"

namespace VoxelEngine
{

class Renderer
{
public:
    explicit Renderer(GLCore::Window& window);
    ~Renderer();

    void RenderScene(const GLCore::Utils::PerspectiveCamera& camera) const;

private:
    void Render(const GLCore::Utils::Shader& shader) const;

    void DepthPass() const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void RenderTerrain(const GLCore::Utils::Shader& shader, const TerrainMeshComponent& terrainComponent) const;
    void RenderLights() const;

private:
    GLCore::Window& m_Window;

    GLuint m_DepthMapFbo;

    GLCore::Utils::Shader* m_PbrShader;
    GLCore::Utils::Shader* m_DepthShader;
    GLCore::Utils::Shader* m_SimpleShader;
};

}

template<typename Component>
using ViewType = decltype(std::declval<entt::registry>().view<Component>());

namespace GLCore::Utils
{

template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const std::string& uniform,
                                                         const ViewType<VoxelEngine::LightComponent>& value) const;
template<>
void Shader::Set<VoxelEngine::Material>(const std::string& uniform, const VoxelEngine::Material& value) const;

}

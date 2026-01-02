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
    void Init() const;

private:
    static void Render(const GLCore::Utils::Shader& shader);

    void PointDepthPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void DepthPass() const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void DrawLights(const GLCore::Utils::PerspectiveCamera& camera) const;

    static void DrawTerrain(const TerrainMeshComponent& mesh,
                            const GLCore::Utils::Shader& shader,
                            const glm::mat4& modelMatrix);
    static void Clear();

private:
    GLCore::Window& m_Window;

    GLuint m_DepthMapFbo;
    GLuint m_DepthMap;

    GLCore::Utils::Shader* m_PbrShader;
    GLCore::Utils::Shader* m_PointDepthShader;
    GLCore::Utils::Shader* m_DepthShader;
    GLCore::Utils::Shader* m_SimpleShader;
};

}

template<typename Component>
using ViewType = decltype(std::declval<entt::registry>().view<Component>());

namespace GLCore::Utils
{

template<>
void Shader::Set<std::vector<VoxelEngine::PointLight> >(const std::string& uniform,
                                                        const std::vector<VoxelEngine::PointLight>& value) const;
template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const std::string& uniform,
                                                         const ViewType<VoxelEngine::LightComponent>& value) const;
template<>
void Shader::Set<VoxelEngine::Material>(const std::string& uniform, const VoxelEngine::Material& value) const;

}

#pragma once

#include <GLCore.hpp>
#include <GLCoreUtils.hpp>

#include "DirectionalLight.hpp"
#include "entt.hpp"
#include "PointLight.hpp"
#include "../Ecs/Components/LightComponent.hpp"
#include "../Ecs/Components/TerrainMeshComponent.hpp"
#include "../Ecs/Components/MeshComponent.hpp"
#include "Target/RenderTarget.hpp"

namespace VoxelEngine
{

}

namespace VoxelEngine
{

enum ShadeType
{
    Albedo,
    Preview,
    Full
};

class Renderer
{
public:
    explicit Renderer();
    ~Renderer();

    void RenderScene(const GLCore::Utils::PerspectiveCamera& camera, const RenderTarget& renderTarget) const;
    void Init() const;

private:
    static void Render(const GLCore::Utils::Shader& shader);

    void BuildSkyboxCubeMap() const;
    void BuildIrradianceCubeMap() const;

    void BuildPreFilterCubeMap() const;
    void BuildBrdfTexture() const;

    void DrawSkybox(const GLCore::Utils::PerspectiveCamera& camera) const;

    void DepthPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void PointDepthPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera, const RenderTarget& renderTarget) const;
    void DrawLights(const GLCore::Utils::PerspectiveCamera& camera) const;

    static void DrawTerrain(const TerrainMeshComponent& mesh,
                            const GLCore::Utils::Shader& shader,
                            const glm::mat4& modelMatrix);
    static void Clear();

private:
    ShadeType m_ShadeType;

    GLuint m_DepthMapFbo = 0;
    GLuint m_DepthMap = 0;

    GLuint m_CaptureFbo = 0;
    GLuint m_CaptureRbo = 0;
    GLuint m_SkyboxCubeMap = 0;
    GLuint m_IrradianceCubeMap = 0;
    GLuint m_PreFilterCubeMap = 0;
    GLuint m_BrdfLutMap = 0;

    GLCore::Utils::Shader* m_PbrShader;
    GLCore::Utils::Shader* m_PointDepthShader;
    GLCore::Utils::Shader* m_DepthShader;
    GLCore::Utils::Shader* m_SimpleShader;

    GLCore::Utils::Shader* m_SkyboxConversionShader;
    GLCore::Utils::Shader* m_SkyboxShader;
    GLCore::Utils::Shader* m_IrradianceMapShader;
    GLCore::Utils::Shader* m_PreFilterConvolutionShader;
    GLCore::Utils::Shader* m_BrdfShader;

    friend class RendererLayer;
};

}

template<typename Component>
using ViewType = decltype(std::declval<entt::registry>().view<Component>());

namespace GLCore::Utils
{

template<>
void Shader::Set<std::vector<VoxelEngine::PointLight> >(const std::vector<VoxelEngine::PointLight>& value) const;
template<>
void Shader::Set<VoxelEngine::DirectionalLight>(const VoxelEngine::DirectionalLight& value) const;
template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const ViewType<VoxelEngine::LightComponent>& value) const;
template<>
void Shader::Set<VoxelEngine::Material>(const VoxelEngine::Material& value) const;

}

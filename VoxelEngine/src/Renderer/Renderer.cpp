#include "Renderer.hpp"

#include <ranges>

#include "../Config.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Ecs/Components/LightComponent.hpp"
#include "../Ecs/Components/TransformComponent.hpp"
#include "GLCore/Utils/ShaderBuilder.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

static constexpr uint32_t MaxPointLights = 16;

namespace VoxelEngine
{

static void CreateDepthCubeMap(GLuint* depthCubeMap);

Renderer::Renderer(Window& window) : m_Window(window), m_DepthMapFbo(0)
{
    m_DepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("point_shadows_depth.vert.glsl"))
            .AddShader(GL_GEOMETRY_SHADER, AssetManager::GetShaderPath("point_shadows_depth.geo.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("point_shadows_depth.frag.glsl"))
            .Build();

    m_PbrShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("pbr.vert.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("pbr.frag.glsl"))
            .Build();

    m_SimpleShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("simple.vert.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("simple.frag.glsl"))
            .Build();

    glGenFramebuffers(1, &m_DepthMapFbo);
}

Renderer::~Renderer() = default;

void Renderer::RenderScene(const PerspectiveCamera& camera)
{
    constexpr glm::vec3 skyColor(0.03f, 0.03f, 0.06f);

    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static bool baked = false;
    if (!baked)
    {
        m_PbrShader->Use();

        std::vector<int32_t> vector{};
        for (int32_t i = 0; i < MaxPointLights; ++i)
            vector.push_back(8 + i);

        m_PbrShader->Set("u_DepthMaps", vector);

        baked = true;
    }

    DepthPass();
    RenderPass(camera);

    // Debug
    m_SimpleShader->Use();
    m_SimpleShader->SetViewProjection(camera.GetViewProjectionMatrix());
    RenderLights();
}

void Renderer::DepthPass() const
{
    const Shader& shader = *m_DepthShader;

    glViewport(0, 0, Config::ShadowWidth, Config::ShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    shader.Use();

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const ViewType<LightComponent>& lightView = registry.view<LightComponent>();

    for (const auto& lightEntity: lightView)
    {
        auto& light = lightView.get<LightComponent>(lightEntity).PointLight;

        if (light.DepthCubeMap == 0)
            CreateDepthCubeMap(&light.DepthCubeMap);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.DepthCubeMap, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.Set("u_LightPosition", light.Position);
        shader.Set("u_FarPlane", Config::ShadowFarPlane);

        std::vector<glm::mat4> shadowTransforms = light.CalculateShadowTransforms();

        for (uint32_t i = 0; i < shadowTransforms.size(); ++i)
            shader.Set("u_ShadowMatrices", shadowTransforms[i], i);

        Render(shader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderPass(const PerspectiveCamera& camera) const
{
    const Shader& shader = *m_PbrShader;

    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    shader.SetViewProjection(camera.GetViewProjectionMatrix());
    shader.Set("u_CameraPosition", camera.GetPosition());
    shader.Set("u_ShadowFarPlane", Config::ShadowFarPlane);

    Render(shader);
}

void Renderer::Render(const Shader& shader) const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    shader.Set("", registry.view<LightComponent>());

    for (const auto& terrainView = registry.view<TerrainComponent>(); const auto& entity: terrainView)
        RenderTerrain(shader, terrainView.get<TerrainComponent>(entity));

    for (const auto view = registry.view<MeshComponent, TransformComponent>(); const auto entity: view)
    {
        auto& mesh = view.get<MeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, transform.Position);
        model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
        model = glm::scale(model, transform.Scale);

        mesh.Model.Draw(shader, model);
    }
}

void Renderer::RenderTerrain(const Shader& shader, const TerrainComponent& terrainComponent) const
{
    glCullFace(GL_FRONT);

    shader.Set("", terrainComponent.TerrainMaterial);

    for (const auto& renderData: terrainComponent.RenderData | std::views::values)
    {
        shader.SetModel(renderData.ModelMatrix);

        glBindVertexArray(renderData.VertexArray);
        glDrawElements(GL_TRIANGLES, renderData.Indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    glCullFace(GL_BACK);
}

void Renderer::RenderLights() const
{
    const Shader& shader = *m_SimpleShader;

    shader.Use();

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const ViewType<LightComponent>& lightView = registry.view<LightComponent>();

    for (const auto& entity: lightView)
    {
        auto light = lightView.get<LightComponent>(entity).PointLight;
        shader.Set("u_Color", light.LightColor);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, light.Position);
        model = glm::scale(model, glm::vec3(0.07f));

        AssetManager::Instance().GetSphereModel().Draw(shader, model);
    }
}

static void CreateDepthCubeMap(GLuint* depthCubeMap)
{
    glGenTextures(1, depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubeMap);

    for (uint32_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_DEPTH_COMPONENT,
                     Config::ShadowWidth,
                     Config::ShadowHeight,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

}

namespace GLCore::Utils
{

template<>
void Shader::Set<std::vector<VoxelEngine::PointLight> >(const std::string&,
                                                        const std::vector<VoxelEngine::PointLight>& value) const
{
    Set<int32_t>("u_PointLightCount", value.size());

    for (int32_t i = 0; i < value.size(); ++i)
    {
        Set(std::format("u_PointLights[{}].LightPosition", i), value[i].Position);
        Set(std::format("u_PointLights[{}].LightColor", i), value[i].LightColor);

        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, value[i].DepthCubeMap);
    }
}

template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const std::string&,
                                                         const ViewType<VoxelEngine::LightComponent>& value) const
{
    Set<int32_t>("u_PointLightCount", value.size());

    int32_t index = 0;
    for (const auto& lightEntity: value)
    {
        auto light = value.get<VoxelEngine::LightComponent>(lightEntity).PointLight;

        Set(std::format("u_PointLights[{}].LightPosition", index), light.Position);
        Set(std::format("u_PointLights[{}].LightColor", index), light.LightColor);

        glActiveTexture(GL_TEXTURE8 + index);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.DepthCubeMap);

        index++;
    }
}

template<>
void Shader::Set<VoxelEngine::Material>(const std::string&, const VoxelEngine::Material& value) const
{
    Set("u_AlbedoFactor", value.AlbedoFactor);
    if (value.AlbedoTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, value.AlbedoTextureId);

        Set("u_HasAlbedoTexture", true);
        Set("u_AlbedoTexture", 0);
    }
    else Set("u_HasAlbedoTexture", false);

    Set("u_MetallicFactor", value.MetallicFactor);
    Set("u_RoughnessFactor", value.RoughnessFactor);
    if (value.MetallicRoughnessTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, value.MetallicRoughnessTextureId);

        Set("u_HasMetallicRoughnessTexture", true);
        Set("u_MetallicRoughnessTexture", 1);
    }
    else Set("u_HasMetallicRoughnessTexture", false);

    if (value.AmbientOcclusionTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, value.AmbientOcclusionTextureId);

        Set("u_HasAmbientOcclusionTexture", true);
        Set("u_AmbientOcclusionTexture", 2);
    }
    else Set("u_HasAmbientOcclusionTexture", false);

    if (value.NormalTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, value.NormalTextureId);

        Set("u_HasNormalTexture", true);
        Set("u_NormalTexture", 3);
    }
    else Set("u_HasNormalTexture", false);
}

}

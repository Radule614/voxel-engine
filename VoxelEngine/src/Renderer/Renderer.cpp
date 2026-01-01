#include "Renderer.hpp"

#include <ranges>

#include "../Config.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Ecs/Components/TransformComponent.hpp"
#include "GLCore/Utils/ShaderBuilder.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

static constexpr uint32_t MaxPointLights = 16;

namespace VoxelEngine
{

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

    constexpr float_t baseHeight = 66.0f;

    m_PointLights.emplace_back(glm::vec3(0.0f, baseHeight + 1.0f, 0.0), glm::vec3(1.0f));
    // m_PointLights.emplace_back(glm::vec3(8.5f, baseHeight + 1.0f, -2.0), glm::vec3(1.0f, 0.0f, 0.0f));
    // m_PointLights.emplace_back(glm::vec3(-8.5f, baseHeight + 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    // m_PointLights.emplace_back(glm::vec3(4.0f, baseHeight + 1.0f, 3.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    // m_PointLights.emplace_back(glm::vec3(-4.0f, baseHeight + 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f));

    glGenFramebuffers(1, &m_DepthMapFbo);
}

Renderer::~Renderer() = default;

void Renderer::RenderScene(const PerspectiveCamera& camera) const
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

std::vector<PointLight>& Renderer::GetPointLights() { return m_PointLights; }

void Renderer::DepthPass() const
{
    const Shader& shader = *m_DepthShader;

    glViewport(0, 0, Config::ShadowWidth, Config::ShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    shader.Use();
    for (const auto& light: m_PointLights)
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.GetDepthCubeMap(), 0);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

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

    for (int32_t i = 0; i < m_PointLights.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLights[i].GetDepthCubeMap());
    }

    Render(shader);
}

void Renderer::Render(const Shader& shader) const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    shader.Set("", m_PointLights);

    for (const auto terrainView = registry.view<TerrainComponent>(); const auto entity: terrainView)
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
    for (auto& pointLight: m_PointLights)
    {
        shader.Set("u_Color", pointLight.LightColor);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, pointLight.Position);
        model = glm::scale(model, glm::vec3(0.07f));

        AssetManager::Instance().GetSphereModel().Draw(shader, model);
    }
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

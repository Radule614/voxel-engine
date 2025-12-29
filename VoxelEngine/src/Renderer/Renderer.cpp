#include "Renderer.hpp"

#include <ranges>

#include "../Assets/AssetManager.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Ecs/Components/TransformComponent.hpp"
#include "GLCore/Utils/ShaderBuilder.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

static constexpr uint32_t ShadowHeight = 2048;
static constexpr uint32_t ShadowWidth = 2048;
static constexpr float_t ShadowNearPlane = 1.0f;
static constexpr float_t ShadowFarPlane = 30.0f;
static constexpr uint32_t MaxPointLights = 16;

namespace VoxelEngine
{

static void CreateDepthCubeMap(GLuint* depthCubeMap);
static std::vector<glm::mat4> CalculateShadowTransforms(glm::vec3 lightPosition);
static void SetPointLightUniformAtIndex(const Shader& shader,
                                        const std::string& uniform,
                                        const PointLight& light,
                                        int32_t index);

Renderer::Renderer(Window& window) : m_Window(window), m_DepthMapFbo(0)
{
    m_TerrainAlbedo = AssetManager::Instance().LoadTexture("assets/textures/atlas.png", "Diffuse");

    m_DepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("point_shadows_depth.vert.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("point_shadows_depth.frag.glsl"))
            .AddShader(GL_GEOMETRY_SHADER, AssetManager::GetShaderPath("point_shadows_depth.geo.glsl"))
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

    m_PointLights = {
        {{0.0f, baseHeight + 1.0f, 0.0}, {1.0f, 1.0f, 1.0f}},
        // {{8.5f, baseHeight + 1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}},
        // {{-8.5f, baseHeight + 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
        // {{4.0f, baseHeight + 1.0f, 3.5f}, {0.0f, 0.0f, 1.0f}},
        // {{-4.0f, baseHeight + 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}},
    };

    glGenFramebuffers(1, &m_DepthMapFbo);
    for (auto& light: m_PointLights)
        CreateDepthCubeMap(&light.DepthCubeMap);
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

        m_PbrShader->Set<std::vector<int32_t> >("u_DepthMaps", vector);

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

    glViewport(0, 0, ShadowWidth, ShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFbo);

    for (const auto& light: m_PointLights)
    {
        shader.Use();

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.DepthCubeMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glClear(GL_DEPTH_BUFFER_BIT);

        shader.Set("u_LightPosition", light.Position);
        shader.Set("u_FarPlane", ShadowFarPlane);

        std::vector<glm::mat4> shadowTransforms = CalculateShadowTransforms(light.Position);

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
    shader.Set("u_ShadowFarPlane", ShadowFarPlane);

    for (int32_t i = 0; i < m_PointLights.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLights[i].DepthCubeMap);
    }

    Render(shader);
}

void Renderer::Render(const Shader& shader) const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    shader.Set<int32_t>("u_PointLightCount", m_PointLights.size());

    for (int32_t i = 0; i < m_PointLights.size(); ++i)
        SetPointLightUniformAtIndex(shader, "u_PointLights", m_PointLights[i], i);

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

    for (const auto& renderData: terrainComponent.RenderData | std::views::values)
    {
        shader.SetModel(renderData.ModelMatrix);

        shader.Set("u_HasAlbedoTexture", true);
        shader.Set("u_HasMetallicRoughnessTexture", false);
        shader.Set("u_HasAmbientOcclusionTexture", false);
        shader.Set("u_HasNormalTexture", false);

        shader.Set("u_AlbedoFactor", glm::vec4(1.0f));
        shader.Set("u_AlbedoTexture", 0);
        shader.Set("u_MetallicFactor", 0.0f);
        shader.Set("u_RoughnessFactor", 0.85f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TerrainAlbedo.id);

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
    for (auto pointLight: m_PointLights)
    {
        shader.Set("u_Color", pointLight.LightColor);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, pointLight.Position);
        model = glm::scale(model, glm::vec3(0.07f));

        AssetManager::Instance().GetSphereModel().Draw(shader, model);
    }
}

static std::vector<glm::mat4> CalculateShadowTransforms(const glm::vec3 lightPosition)
{
    static float_t aspect = (float_t) ShadowWidth / (float_t) ShadowHeight;
    static glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, ShadowNearPlane, ShadowFarPlane);

    std::vector<glm::mat4> shadowTransforms{};
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(-1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(0.0, 1.0, 0.0),
                                                        glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(0.0, -1.0, 0.0),
                                                        glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(0.0, 0.0, 1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition,
                                                        lightPosition + glm::vec3(0.0, 0.0, -1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));

    return shadowTransforms;
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
                     ShadowWidth,
                     ShadowHeight,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
}

static void SetPointLightUniformAtIndex(const Shader& shader,
                                        const std::string& uniform,
                                        const PointLight& light,
                                        const int32_t index)
{
    shader.Set(std::format("{}[{}].LightPosition", uniform, index), light.Position);
    shader.Set(std::format("{}[{}].LightColor", uniform, index), light.LightColor);
}

}

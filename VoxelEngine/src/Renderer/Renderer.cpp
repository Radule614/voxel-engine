#include "Renderer.hpp"

#include "DirectionalLight.hpp"
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

static glm::mat4 CalculateModelMatrix(const TransformComponent& transform);
static void CreateDepthMap(GLuint* depthMap);
static void CreateDepthCubeMap(GLuint* depthCubeMap);
static std::vector<PointLight> GetCloseLights(glm::vec3 position,
                                              ViewType<LightComponent> lightView,
                                              bool shouldOffsetChunk = false);

static DirectionalLight DirLight(glm::vec3(0.0f, -1.0f, 0.3f), 2.0f, glm::vec3(1.0f, 0.97f, 0.92f));

Renderer::Renderer(Window& window) : m_Window(window), m_DepthMapFbo(0), m_DepthMap(0)
{
    m_PointDepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("point_shadows_depth.vert.glsl"))
            .AddShader(GL_GEOMETRY_SHADER, AssetManager::GetShaderPath("point_shadows_depth.geo.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("point_shadows_depth.frag.glsl"))
            .Build();

    m_DepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, AssetManager::GetShaderPath("depth.vert.glsl"))
            .AddShader(GL_FRAGMENT_SHADER, AssetManager::GetShaderPath("depth.frag.glsl"))
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
    CreateDepthMap(&m_DepthMap);

    // auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    // PointLight pointLight(glm::vec3(2.0f, 67.0f, 0.0), glm::vec3(1.0f));
    // registry.emplace<LightComponent>(registry.create(), pointLight);
}

Renderer::~Renderer() = default;

void Renderer::Init() const
{
    m_PbrShader->Use();

    std::vector<int32_t> vector{};
    for (int32_t i = 0; i < MaxPointLights; ++i)
        vector.push_back(8 + i);

    m_PbrShader->Set("u_DepthMaps", vector);
    m_PbrShader->Set("u_DepthMap", 7);
}

void Renderer::RenderScene(const PerspectiveCamera& camera) const
{
    Clear();

    DepthPass(camera);
    PointDepthPass(camera);
    RenderPass(camera);
    DrawLights(camera);
}

void Renderer::DepthPass(const PerspectiveCamera& camera) const
{
    const Shader& shader = *m_DepthShader;

    glViewport(0, 0, Config::ShadowWidth, Config::ShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    shader.Use();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    const auto position = DirLight.GetLightSpaceTransform(camera.GetPosition());
    shader.Set("u_LightSpaceMatrix", position);

    Render(shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PointDepthPass(const PerspectiveCamera& camera) const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const ViewType<LightComponent>& view = registry.view<LightComponent>();
    if (view.empty())
        return;

    const Shader& shader = *m_PointDepthShader;

    glViewport(0, 0, Config::PointShadowWidth, Config::PointShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    shader.Use();

    static size_t lightIndex = 0;

    const auto entity = view.begin()[lightIndex % view.size()];
    auto& light = view.get<LightComponent>(entity).PointLight;

    if (light.DepthCubeMap == 0)
        CreateDepthCubeMap(&light.DepthCubeMap);

    if (glm::length(light.Position - camera.GetPosition()) < 100.0f)
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.DepthCubeMap, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.Set("u_LightPosition", light.Position);
        shader.Set("u_FarPlane", Config::PointShadowFarPlane);

        const std::vector<glm::mat4> shadowTransforms = light.CalculateShadowTransforms();

        for (uint32_t i = 0; i < shadowTransforms.size(); ++i)
            shader.Set("u_ShadowMatrices", shadowTransforms[i], i);

        Render(shader);
    }

    ++lightIndex %= 500000;

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
    shader.Set("u_ShadowFarPlane", Config::PointShadowFarPlane);

    shader.Set("u_LightSpaceMatrix", DirLight.GetLightSpaceTransform(camera.GetPosition()));
    shader.Set("", DirLight);
    shader.Set("u_HasDirectionalLight", true);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_DepthMap);

    Render(shader);
}

void Renderer::Render(const Shader& shader)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto lightView = registry.view<LightComponent>();

    for (const auto& view = registry.view<TerrainMeshComponent, TransformComponent>(); const auto& entity: view)
    {
        auto& mesh = view.get<TerrainMeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        shader.Set("", GetCloseLights(transform.Position, lightView, true));

        DrawTerrain(mesh, shader, CalculateModelMatrix(transform));
    }

    for (const auto view = registry.view<MeshComponent, TransformComponent>(); const auto entity: view)
    {
        auto& mesh = view.get<MeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        shader.Set("", GetCloseLights(transform.Position, lightView));

        mesh.Model.Draw(shader, CalculateModelMatrix(transform));
    }
}

void Renderer::DrawTerrain(const TerrainMeshComponent& mesh, const Shader& shader, const glm::mat4& modelMatrix)
{
    glCullFace(GL_FRONT);

    shader.Set("", mesh.Material);
    shader.SetModel(modelMatrix);

    glBindVertexArray(mesh.VertexArray);
    glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glCullFace(GL_BACK);
}

void Renderer::DrawLights(const PerspectiveCamera& camera) const
{
    const Shader& shader = *m_SimpleShader;

    shader.Use();
    shader.SetViewProjection(camera.GetViewProjectionMatrix());

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const ViewType<LightComponent>& lightView = registry.view<LightComponent>();

    for (const auto& entity: lightView)
    {
        auto light = lightView.get<LightComponent>(entity).PointLight;
        if (!lightView.contains(entity))
            continue;

        shader.Set("u_Color", light.LightColor);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, light.Position);
        model = glm::scale(model, glm::vec3(0.07f));

        AssetManager::Instance().GetSphereModel().Draw(shader, model);
    }
}

void Renderer::Clear()
{
    // constexpr glm::vec3 skyColor(0.03f, 0.03f, 0.06f);
    // glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static glm::mat4 CalculateModelMatrix(const TransformComponent& transform)
{
    auto model = glm::mat4(1.0);

    model = glm::translate(model, transform.Position);
    model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
    model = glm::scale(model, transform.Scale);

    return model;
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
                     Config::PointShadowWidth,
                     Config::PointShadowHeight,
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

static void CreateDepthMap(GLuint* depthMap)
{
    glGenTextures(1, depthMap);
    glBindTexture(GL_TEXTURE_2D, *depthMap);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 Config::ShadowWidth,
                 Config::ShadowHeight,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

static std::vector<PointLight> GetCloseLights(const glm::vec3 position,
                                              const ViewType<LightComponent> lightView,
                                              const bool shouldOffsetChunk)
{
    std::vector<PointLight> closeLights{};

    for (const auto lightEntity: lightView)
    {
        if (!lightView.contains(lightEntity))
            continue;

        auto& light = lightView.get<LightComponent>(lightEntity).PointLight;
        glm::vec3 lightPosition = light.Position;
        glm::vec3 temp = position;

        if (shouldOffsetChunk)
        {
            lightPosition.y = 0.0f;
            temp.x += CHUNK_WIDTH / 2;
            temp.y = 0.0f;
            temp.z += CHUNK_WIDTH / 2;
        }

        if (glm::length(lightPosition - temp) <= Config::PointShadowFarPlane + 7.0f)
            closeLights.push_back(light);
    }

    return closeLights;
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
        const auto& light = value[i];

        Set(std::format("u_PointLights[{}].LightPosition", i), light.Position);
        Set(std::format("u_PointLights[{}].LightColor", i), light.LightColor);

        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.DepthCubeMap);
    }
}

template<>
void Shader::Set<VoxelEngine::DirectionalLight>(const std::string&, const VoxelEngine::DirectionalLight& value) const
{
    Set("u_DirectionalLight.LightDirection", value.Direction);
    Set("u_DirectionalLight.LightColor", value.LightColor);
    Set("u_DirectionalLight.LightIntensity", value.LightIntensity);
}

template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const std::string&,
                                                         const ViewType<VoxelEngine::LightComponent>& value) const
{
    Set<int32_t>("u_PointLightCount", value.size());

    int32_t index = 0;
    for (const auto& lightEntity: value)
    {
        const auto& light = value.get<VoxelEngine::LightComponent>(lightEntity).PointLight;

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

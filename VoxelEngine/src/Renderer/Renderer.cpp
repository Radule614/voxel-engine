#include "Renderer.hpp"

#include "DirectionalLight.hpp"
#include "../Config.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Ecs/Components/TransformComponent.hpp"
#include "../Ecs/Components/SkinComponent.hpp"
#include "GLCore/Utils/ShaderBuilder.hpp"
#include "Utils/RenderUtils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

static constexpr uint32_t MaxPointLights = 16;

namespace VoxelEngine
{

static void CreateEnvCubeMap(GLuint* envCubeMap, uint32_t size);
static void CreatePreFilterMap(GLuint* preFilterMap);
static void CreateDepthMap(GLuint* depthMap);
static void CreateDepthCubeMap(GLuint* depthCubeMap);
static void CreateBrdfLutMap(GLuint* bdrLutTexture);
static std::vector<PointLight> GetCloseLights(glm::vec3 position,
                                              ViewType<LightComponent> lightView,
                                              bool shouldOffsetChunk = false);

static glm::mat4 CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
static glm::mat4 CaptureViews[] =
{
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

static DirectionalLight DirLight(glm::vec3(0.0f, -1.0f, 0.3f), 2.0f, glm::vec3(1.0f, 0.97f, 0.92f));

Renderer::Renderer() : m_ShadeType(Preview)
{
    m_PointDepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "shadows/point_shadows_depth.vert.glsl")
            .AddShader(GL_GEOMETRY_SHADER, "shadows/point_shadows_depth.geo.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "shadows/point_shadows_depth.frag.glsl")
            .Build();

    m_DepthShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "shadows/depth.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "shadows/depth.frag.glsl")
            .Build();

    m_PbrShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/pbr.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/pbr.frag.glsl")
            .Build();

    m_SimpleShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "simple.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "simple.frag.glsl")
            .Build();

    m_SkyboxConversionShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/cubemap.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/skybox_conversion.frag.glsl")
            .Build();

    m_SkyboxShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/skybox.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/skybox.frag.glsl")
            .Build();

    m_IrradianceMapShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/cubemap.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/irradiance_map.frag.glsl")
            .Build();

    m_PreFilterConvolutionShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/cubemap.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/pre_filter_convolution.frag.glsl")
            .Build();

    m_BrdfShader = ShaderBuilder()
            .AddShader(GL_VERTEX_SHADER, "lighting/brdf.vert.glsl")
            .AddShader(GL_FRAGMENT_SHADER, "lighting/brdf.frag.glsl")
            .Build();

    glGenFramebuffers(1, &m_DepthMapFbo);
    CreateEnvCubeMap(&m_SkyboxCubeMap, 512);
    CreateEnvCubeMap(&m_IrradianceCubeMap, 64);
    CreatePreFilterMap(&m_PreFilterCubeMap);
    CreateDepthMap(&m_DepthMap);
    CreateBrdfLutMap(&m_BrdfLutMap);

    glGenFramebuffers(1, &m_CaptureFbo);
    glGenRenderbuffers(1, &m_CaptureRbo);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFbo);

    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    m_PbrShader->Set("u_IrradianceMap", 6);
    m_PbrShader->Set("u_PrefilterMap", 5);
    m_PbrShader->Set("u_BrdfLut", 4);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_FRONT);
    BuildSkyboxCubeMap();
    BuildIrradianceCubeMap();
    BuildPreFilterCubeMap();
    glCullFace(GL_BACK);
    BuildBrdfTexture();
}

void Renderer::RenderScene(const PerspectiveCamera& camera, const RenderTarget& renderTarget) const
{
    Clear();

    DepthPass(camera);
    PointDepthPass(camera);

    renderTarget.Bind();

    RenderPass(camera);
    DrawLights(camera);
    DrawSkybox(camera);

    renderTarget.Unbind();
}

void Renderer::DrawPrimitives(const Shader& shader,
                              const std::vector<RenderPrimitive>& primitives,
                              const glm::mat4& model,
                              const std::vector<glm::mat4>* jointMatrices)
{
    for (auto [Vao, Mode, IndexCount, IndexType, IndexOffset, Material]: primitives)
    {
        glBindVertexArray(Vao);

        shader.Set("u_IsSkinned", jointMatrices != nullptr);
        if (jointMatrices != nullptr)
            shader.Set("u_JointMatrices", *jointMatrices);

        shader.SetModel(model);
        shader.Set(Material);

        glDrawElements(Mode, IndexCount, IndexType, IndexOffset);
    }

    glBindVertexArray(0);
}

void Renderer::BuildSkyboxCubeMap() const
{
    static Texture skyboxTexture = AssetManager::Instance().LoadHdrTexture("assets/hdr/day_pure_sky.hdr");

    const Shader& shader = *m_SkyboxConversionShader;

    shader.Use();
    shader.Set("u_Projection", CaptureProjection);
    shader.Set("u_EquirectangularMap", 6);
    shader.Set("u_Strength", 1.0f);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture.Id);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFbo);

    for (size_t i = 0; i < 6; ++i)
    {
        shader.Set("u_View", CaptureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_SkyboxCubeMap,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawPrimitives(shader, AssetManager::Instance().GetCubeModel().GetMeshPrimitives(0), glm::mat4(1.0));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BuildIrradianceCubeMap() const
{
    const Shader& shader = *m_IrradianceMapShader;

    shader.Use();
    shader.Set("u_Projection", CaptureProjection);
    shader.Set("u_EnvironmentMap", 6);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxCubeMap);

    glViewport(0, 0, 64, 64);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64);

    for (size_t i = 0; i < 6; ++i)
    {
        shader.Set("u_View", CaptureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_IrradianceCubeMap,
                               0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderUtils::DrawCube(shader, glm::mat4(1.0));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BuildPreFilterCubeMap() const
{
    const Shader& shader = *m_PreFilterConvolutionShader;

    shader.Use();
    shader.Set("u_Projection", CaptureProjection);
    shader.Set("u_EnvironmentMap", 6);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxCubeMap);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFbo);
    uint32_t maxMipLevels = 5;
    for (size_t mip = 0; mip < maxMipLevels; ++mip)
    {
        uint32_t mipWidth = 128 >> mip;
        uint32_t mipHeight = 128 >> mip;

        glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float_t roughness = (float_t) mip / (float_t) (maxMipLevels - 1);
        shader.Set("u_Roughness", roughness);

        for (size_t i = 0; i < 6; ++i)
        {
            shader.Set("u_View", CaptureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   m_PreFilterCubeMap,
                                   mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            RenderUtils::DrawCube(shader, glm::mat4(1.0));
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BuildBrdfTexture() const
{
    m_BrdfShader->Use();

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrdfLutMap, 0);

    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderUtils::DrawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkybox(const PerspectiveCamera& camera) const
{
    const Shader& shader = *m_SkyboxShader;

    shader.Use();
    shader.Set("u_View", camera.GetViewMatrix());
    shader.Set("u_Projection", camera.GetProjectionMatrix());
    shader.Set("u_EnvironmentMap", 6);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxCubeMap);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);

    RenderUtils::DrawCube(shader, glm::mat4(1.0));

    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
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

    ++lightIndex %= 8192;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderPass(const PerspectiveCamera& camera) const
{
    const Shader& shader = *m_PbrShader;

    shader.Use();
    shader.SetViewProjection(camera.GetViewProjectionMatrix());
    shader.Set("u_CameraPosition", camera.GetPosition());
    shader.Set("u_ShadowFarPlane", Config::PointShadowFarPlane);

    shader.Set("u_LightSpaceMatrix", DirLight.GetLightSpaceTransform(camera.GetPosition()));
    shader.Set(DirLight);

    shader.Set<int32_t>("u_ShadeType", m_ShadeType);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_DepthMap);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceCubeMap);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PreFilterCubeMap);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_BrdfLutMap);

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

        shader.Set(GetCloseLights(transform.WorldPosition, lightView, true));

        DrawTerrain(mesh, shader, transform.WorldMatrix);
    }

    for (const auto view = registry.view<MeshComponent, TransformComponent>(); const auto entity: view)
    {
        auto& mesh = view.get<MeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        shader.Set(GetCloseLights(transform.WorldPosition, lightView));

        const std::vector<glm::mat4>* jointMatrices = nullptr;
        const SkinComponent* skinComponent = registry.try_get<SkinComponent>(entity);
        if (skinComponent != nullptr && skinComponent->IsEnabled)
            jointMatrices = &skinComponent->JointMatrices;

        DrawPrimitives(shader, mesh.Primitives, transform.WorldMatrix, jointMatrices);
    }
}

void Renderer::DrawTerrain(const TerrainMeshComponent& mesh, const Shader& shader, const glm::mat4& modelMatrix)
{
    shader.Set(mesh.Material);
    shader.SetModel(modelMatrix);
    shader.Set("u_IsSkinned", false);

    glBindVertexArray(mesh.VertexArray);
    glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
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

        RenderUtils::DrawSphere(shader, model);
    }
}

void Renderer::Clear()
{
    glClearColor(0, 0, 0, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

static void CreateEnvCubeMap(GLuint* envCubeMap, const uint32_t size)
{
    glGenTextures(1, envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *envCubeMap);

    for (size_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

static void CreatePreFilterMap(GLuint* preFilterMap)
{
    glGenTextures(1, preFilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *preFilterMap);

    for (size_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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

static void CreateBrdfLutMap(GLuint* bdrLutTexture)
{
    glGenTextures(1, bdrLutTexture);

    glBindTexture(GL_TEXTURE_2D, *bdrLutTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
void Shader::Set<std::vector<VoxelEngine::PointLight> >(const std::vector<VoxelEngine::PointLight>& value) const
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
void Shader::Set<VoxelEngine::DirectionalLight>(const VoxelEngine::DirectionalLight& value) const
{
    Set("u_HasDirectionalLight", true);
    Set("u_DirectionalLight.LightDirection", value.Direction);
    Set("u_DirectionalLight.LightColor", value.LightColor);
    Set("u_DirectionalLight.LightIntensity", value.LightIntensity);
}

template<>
void Shader::Set<ViewType<VoxelEngine::LightComponent> >(const ViewType<VoxelEngine::LightComponent>& value) const
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
void Shader::Set<VoxelEngine::Material>(const VoxelEngine::Material& value) const
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

template<>
void Shader::Set<std::vector<glm::mat4> >(const std::string& uniform, const std::vector<glm::mat4>& value) const
{
    for (int32_t i = 0; i < value.size(); ++i) { Set<glm::mat4>(fmt::format("{}[{}]", uniform, i), value[i]); }
}

}

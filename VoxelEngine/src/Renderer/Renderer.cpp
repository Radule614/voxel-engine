#include "Renderer.hpp"

#include <ranges>

#include "../Assets/AssetManager.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Ecs/Components/TransformComponent.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static void SetDirectionalLightUniform(const Shader& shader,
                                       const std::string& uniform,
                                       const DirectionalLight& light);

static void SetPointLightUniformAtIndex(const Shader& shader,
                                        const std::string& uniform,
                                        const PointLight& light,
                                        int32_t index);

void Renderer::Init(Window& window) { g_Renderer = new Renderer(window); }

void Renderer::Shutdown()
{
    delete g_Renderer;
    g_Renderer = nullptr;
}

Renderer& Renderer::Instance() { return *g_Renderer; }

Renderer::Renderer(Window& window) : m_Window(window)
{
    m_TextureAtlas = AssetManager::Instance().LoadTexture("assets/textures/atlas.png", "Diffuse");

    m_TerrainShader = Shader::FromGLSLTextFiles("assets/shaders/voxel.vert.glsl", "assets/shaders/voxel.frag.glsl");
    m_MeshShader = Shader::FromGLSLTextFiles("assets/shaders/pbr.vert.glsl", "assets/shaders/pbr.frag.glsl");
    m_SimpleShader = Shader::FromGLSLTextFiles("assets/shaders/simple.vert.glsl", "assets/shaders/simple.frag.glsl");

    const DirectionalLight light = {
        glm::normalize(glm::vec3(1.0f, -2.0f, 1.0f)),
        glm::vec3(0.5f),
        glm::vec3(1.0f),
        glm::vec3(0.1f)
    };

    m_DirectionalLight = light;

    m_PointLights = {
        {{8.5f, 1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}},
        {{-8.5f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
        {{4.0f, 1.0f, 3.5f}, {0.0f, 0.0f, 1.0f}},
        {{-4.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}},
    };
}

void Renderer::SetDirectionalLight(const DirectionalLight& light) { m_DirectionalLight = light; }

void Renderer::RenderScene(const PerspectiveCamera& camera) const { RenderPass(camera); }

void Renderer::Render(const PerspectiveCamera& camera) const
{
    constexpr glm::vec3 nightColor(0.1f);
    constexpr glm::vec3 dayColor(0.14f, 0.59f, 0.74f);

    const float_t ratio = (TerrainConfig::SunRadiance - 1.0f) / TerrainConfig::MaxRadiance;
    const auto skyColor = glm::mix(nightColor, dayColor, ratio);

    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto terrainView = registry.view<TerrainComponent>(); const auto entity: terrainView)
    {
        auto& renderDataMap = terrainView.get<TerrainComponent>(entity).RenderData;
        RenderTerrain(renderDataMap);
    }

    for (const auto view = registry.view<MeshComponent, TransformComponent>(); const auto entity: view)
    {
        auto& mesh = view.get<MeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, transform.Position);
        model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
        model = glm::scale(model, transform.Scale);

        RenderMesh(mesh, camera, model, *m_MeshShader);
    }

    RenderLights();
}

void Renderer::RenderPass(const PerspectiveCamera& camera) const
{
    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_TerrainShader->GetRendererID());
    m_TerrainShader->SetViewProjection(camera.GetViewProjectionMatrix());
    glUseProgram(m_MeshShader->GetRendererID());
    m_MeshShader->SetViewProjection(camera.GetViewProjectionMatrix());
    glUseProgram(m_SimpleShader->GetRendererID());
    m_SimpleShader->SetViewProjection(camera.GetViewProjectionMatrix());

    Render(camera);
}

void Renderer::RenderMesh(const MeshComponent& meshComponent,
                          const PerspectiveCamera& camera,
                          const glm::mat4& model,
                          const Shader& shader) const
{
    glUseProgram(shader.GetRendererID());

    SetDirectionalLightUniform(shader, "u_DirectionalLight", m_DirectionalLight);
    shader.SetVec3("u_CameraPosition", camera.GetPosition());

    for (int32_t i = 0; i < m_PointLights.size(); ++i)
        SetPointLightUniformAtIndex(shader, "u_PointLights", m_PointLights[i], i);

    meshComponent.Model.Draw(shader, model);

    glUseProgram(0);
}

void Renderer::RenderTerrain(const std::unordered_map<Position2D, ChunkRenderData>& renderDataMap) const
{
    const Shader& shader = *m_TerrainShader;

    glUseProgram(shader.GetRendererID());
    glCullFace(GL_FRONT);

    for (const auto& metadata: renderDataMap | std::views::values)
    {
        shader.SetModel(metadata.ModelMatrix);

        shader.SetInt("u_MaxRadiance", TerrainConfig::MaxRadiance);
        shader.SetInt("u_RadianceGridWidth", RADIANCE_WIDTH);
        shader.SetInt("u_RadianceGridHeight", RADIANCE_HEIGHT);
        shader.SetInt("u_Atlas", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

        glBindVertexArray(metadata.VertexArray);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, metadata.RadianceStorageBuffer);
        glDrawElements(GL_TRIANGLES, metadata.Indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindVertexArray(0);
    }

    glCullFace(GL_BACK);
}

void Renderer::RenderLights() const
{
    const Shader& shader = *m_SimpleShader;

    glUseProgram(shader.GetRendererID());

    for (auto pointLight: m_PointLights)
    {
        shader.SetVec3("u_Color", pointLight.LightColor);

        auto model = glm::mat4(1.0);
        model = glm::translate(model, pointLight.Position);
        model = glm::scale(model, glm::vec3(0.07f));

        AssetManager::Instance().GetSphereModel().Draw(shader, model);
    }
    glUseProgram(0);
}

static void SetPointLightUniformAtIndex(const Shader& shader,
                                        const std::string& uniform,
                                        const PointLight& light,
                                        const int32_t index)
{
    shader.SetVec3(std::format("{}[{}].LightPosition", uniform, index), light.Position);
    shader.SetVec3(std::format("{}[{}].LightColor", uniform, index), light.LightColor);
}

static void SetDirectionalLightUniform(const Shader& shader,
                                       const std::string& uniform,
                                       const DirectionalLight& light)
{
    shader.SetVec3(uniform + ".Direction", light.Direction);
    shader.SetVec3(uniform + ".Ambient", light.Ambient);
    shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
    shader.SetVec3(uniform + ".Specular", light.Specular);
}

}

#include "Renderer.hpp"
#include "../Utils/Utils.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Terrain/TerrainConfig.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

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
    m_MeshShader = Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");

    const DirectionalLight light = {
        glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::vec3(0.25f),
        glm::vec3(1.0f),
        glm::vec3(0.1f)
    };

    m_DirectionalLight = light;
}

void Renderer::SetDirectionalLight(const DirectionalLight& light) { m_DirectionalLight = light; }

void Renderer::RenderScene(const PerspectiveCamera& camera) const { RenderPass(camera); }

void Renderer::Render(const PerspectiveCamera& camera, const Shader* terrainShader, const Shader* meshShader) const
{
    constexpr glm::vec3 nightColor(0.1f);
    constexpr glm::vec3 dayColor(0.14f, 0.59f, 0.74f);

    const float ratio = (TerrainConfig::SunRadiance - 1.0f) / TerrainConfig::MaxRadiance;
    const auto skyColor = glm::mix(nightColor, dayColor, ratio);

    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto terrainView = registry.view<TerrainComponent>(); const auto entity: terrainView)
    {
        auto& renderDataMap = terrainView.get<TerrainComponent>(entity).RenderData;
        RenderTerrain(renderDataMap, terrainShader);
    }

    glCullFace(GL_BACK);
    for (const auto view = registry.view<MeshComponent, TransformComponent>(); const auto entity: view)
    {
        auto& mesh = view.get<MeshComponent>(entity);
        auto& transform = view.get<TransformComponent>(entity);
        auto model = glm::mat4(1.0);
        model = glm::translate(model, transform.Position);
        model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
        model = glm::scale(model, transform.Scale);
        RenderMesh(mesh, camera, model, meshShader);
    }
    glCullFace(GL_FRONT);
}

void Renderer::RenderPass(const PerspectiveCamera& camera) const
{
    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_TerrainShader->GetRendererID());
    m_TerrainShader->SetViewProjection(camera.GetViewProjectionMatrix());
    glUseProgram(m_MeshShader->GetRendererID());
    m_MeshShader->SetViewProjection(camera.GetViewProjectionMatrix());

    glActiveTexture(GL_TEXTURE0);
    Render(camera, m_TerrainShader, m_MeshShader);
}

void Renderer::RenderMesh(const MeshComponent& meshComponent,
                          const PerspectiveCamera& camera,
                          const glm::mat4& model,
                          const Shader* shader) const
{
    glUseProgram(shader->GetRendererID());
    shader->SetVec3("u_CameraPos", camera.GetPosition());
    shader->SetModel(model);
    SetDirectionalLightUniform(*shader, "u_DirectionalLight", m_DirectionalLight);
    for (const Mesh& mesh: meshComponent.Meshes)
    {
        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        uint32_t normalNr = 1;
        const auto& textures = mesh.GetTextures();
        for (size_t i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].type;
            if (name == "Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "Specular")
                number = std::to_string(specularNr++);
            else if (name == "Normal")
                number = std::to_string(normalNr++);
            const std::string uniform = "u_Texture" + name + "_" + number;
            glUniform1i(glGetUniformLocation(shader->GetRendererID(), uniform.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(mesh.GetVAO());
        glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

void Renderer::RenderTerrain(const std::unordered_map<Position2D, ChunkRenderData>& renderDataMap,
                             const Shader* shader) const
{
    glUseProgram(shader->GetRendererID());
    for (const auto& [position, metadata]: renderDataMap)
    {
        shader->SetModel(metadata.ModelMatrix);

        shader->SetInt("u_MaxRadiance", TerrainConfig::MaxRadiance);
        shader->SetInt("u_RadianceGridWidth", RADIANCE_WIDTH);
        shader->SetInt("u_RadianceGridHeight", RADIANCE_HEIGHT);
        shader->SetInt("u_Atlas", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

        glBindVertexArray(metadata.VertexArray);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, metadata.RadianceStorageBuffer);
        glDrawElements(GL_TRIANGLES, metadata.Indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindVertexArray(0);
    }
}

void Renderer::SetPointLightUniform(const Shader& shader, const std::string& uniform, const PointLight& light)
{
    shader.SetVec3(uniform + ".Position", light.Position);
    shader.SetVec3(uniform + ".Ambient", light.Ambient);
    shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
    shader.SetVec3(uniform + ".Specular", light.Specular);
    shader.SetFloat(uniform + ".Constant", light.Constant);
    shader.SetFloat(uniform + ".Linear", light.Linear);
    shader.SetFloat(uniform + ".Quadratic", light.Quadratic);
}

void Renderer::SetDirectionalLightUniform(const Shader& shader,
                                          const std::string& uniform,
                                          const DirectionalLight& light)
{
    shader.SetVec3(uniform + ".Direction", light.Direction);
    shader.SetVec3(uniform + ".Ambient", light.Ambient);
    shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
    shader.SetVec3(uniform + ".Specular", light.Specular);
}

void Renderer::SetSpotLightUniform(const Shader& shader, const std::string& uniform, const SpotLight& light)
{
    shader.SetVec3(uniform + ".Direction", light.Direction);
    shader.SetVec3(uniform + ".Position", light.Position);
    shader.SetVec3(uniform + ".Ambient", light.Ambient);
    shader.SetVec3(uniform + ".Diffuse", light.Diffuse);
    shader.SetVec3(uniform + ".Specular", light.Specular);
    shader.SetFloat(uniform + ".CutOff", light.CutOff);
    shader.SetFloat(uniform + ".OuterCutOff", light.OuterCutOff);
    shader.SetFloat(uniform + ".Constant", light.Constant);
    shader.SetFloat(uniform + ".Linear", light.Linear);
    shader.SetFloat(uniform + ".Quadratic", light.Quadratic);
}

}

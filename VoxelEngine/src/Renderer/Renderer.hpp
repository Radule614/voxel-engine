#pragma once

#include <GLCore.hpp>
#include <GLCoreUtils.hpp>

#include "../Terrain/Utils/Position2D.hpp"
#include "../Ecs/Components/TerrainComponent.hpp"
#include "../Ecs/Components/MeshComponent.hpp"
#include "../Assets/Texture.hpp"

namespace VoxelEngine
{

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 LightColor;

    PointLight(const glm::vec3 position, const glm::vec3 color) : Position(position), LightColor(color)
    {
    }
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};

class Renderer
{
public:
    static void Init(GLCore::Window& window);
    static void Shutdown();
    static Renderer& Instance();

    explicit Renderer(GLCore::Window& window);

    void SetDirectionalLight(const DirectionalLight& light);
    void RenderScene(const GLCore::Utils::PerspectiveCamera& camera) const;

private:
    void Render(const GLCore::Utils::PerspectiveCamera& camera) const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;

    void RenderMesh(const MeshComponent& meshComponent,
                    const GLCore::Utils::PerspectiveCamera& camera,
                    const glm::mat4& model,
                    const GLCore::Utils::Shader& shader) const;

    void RenderTerrain(const std::unordered_map<Position2D, ChunkRenderData>& renderDataMap) const;

    void RenderLights() const;

private:
    GLCore::Window& m_Window;
    Texture m_TextureAtlas;
    DirectionalLight m_DirectionalLight{};

    GLCore::Utils::Shader* m_TerrainShader;
    GLCore::Utils::Shader* m_MeshShader;
    GLCore::Utils::Shader* m_SimpleShader;

    std::vector<PointLight> m_PointLights;
};

inline Renderer* g_Renderer = nullptr;

}

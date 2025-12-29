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
    GLuint DepthCubeMap = 0;

    PointLight(const glm::vec3 position, const glm::vec3 color) : Position(position), LightColor(color)
    {
    }
};

class Renderer
{
public:
    explicit Renderer(GLCore::Window& window);
    ~Renderer();

    void RenderScene(const GLCore::Utils::PerspectiveCamera& camera) const;
    std::vector<PointLight>& GetPointLights();

private:
    void Render(const GLCore::Utils::Shader& shader) const;

    void DepthPass() const;
    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;
    void RenderTerrain(const GLCore::Utils::Shader& shader,
                       const std::unordered_map<Position2D, ChunkRenderData>& renderDataMap) const;
    void RenderLights() const;

private:
    GLCore::Window& m_Window;
    Texture m_TextureAtlas;

    std::vector<PointLight> m_PointLights;

    GLuint m_DepthMapFbo;

    GLCore::Utils::Shader* m_PbrShader;
    GLCore::Utils::Shader* m_DepthShader;
    GLCore::Utils::Shader* m_SimpleShader;
};

}

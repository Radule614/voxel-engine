#pragma once

#include <GLCore.hpp>
#include <GLCoreUtils.hpp>

#include "../Terrain/Utils/Position2D.hpp"
#include "../Ecs/Components/TerrainComponent.hpp"
#include "../Ecs/Components/MeshComponent.hpp"
#include "../Assets/Texture.hpp"

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

namespace VoxelEngine
{

struct Material
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float_t Shininess;
};

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float_t Constant;
    float_t Linear;
    float_t Quadratic;

    PointLight(glm::vec3 pos, glm::vec3 color)
    {
        Position = pos;
        Ambient = color;
        Diffuse = color;
        Specular = color;
        Constant = 0.005f;
        Linear = 0.005f;
        Quadratic = 0.012f;
    }

    PointLight(glm::vec3 pos, glm::vec3 a, glm::vec3 d, glm::vec3 s, float_t x, float_t y, float_t z)
    {
        Position = pos;
        Ambient = a;
        Diffuse = d;
        Specular = s;
        Constant = x;
        Linear = y;
        Quadratic = z;
    }
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};

struct SpotLight
{
    glm::vec3 Direction;
    glm::vec3 Position;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float_t CutOff;
    float_t OuterCutOff;
    float_t Constant;
    float_t Linear;
    float_t Quadratic;

    SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 dif, glm::vec3 spec)
    {
        Position = pos;
        Direction = dir;
        Ambient = glm::vec3(0.0f);
        Diffuse = dif;
        Specular = spec;
        CutOff = glm::cos(glm::radians(2.0f));
        OuterCutOff = glm::cos(glm::radians(3.0f));
        Constant = 0;
        Linear = 0.009f;
        Quadratic = 0;
    }
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
    void Render(const GLCore::Utils::PerspectiveCamera& camera,
                const GLCore::Utils::Shader* terrainShader,
                const GLCore::Utils::Shader* meshShader) const;

    void RenderPass(const GLCore::Utils::PerspectiveCamera& camera) const;

    void RenderMesh(const MeshComponent& meshComponent,
                    const GLCore::Utils::PerspectiveCamera& camera,
                    const glm::mat4& model,
                    const GLCore::Utils::Shader* shader) const;
    void RenderTerrain(const std::unordered_map<Position2D, ChunkRenderData>& renderDataMap,
                       const GLCore::Utils::Shader* shader) const;

    static void SetPointLightUniform(const GLCore::Utils::Shader& shader, const std::string&, const PointLight&);

    static void SetDirectionalLightUniform(const GLCore::Utils::Shader& shader,
                                           const std::string&,
                                           const DirectionalLight&);

    static void SetSpotLightUniform(const GLCore::Utils::Shader& shader, const std::string&, const SpotLight&);

private:
    GLCore::Window& m_Window;
    Texture m_TextureAtlas;
    DirectionalLight m_DirectionalLight{};

    GLCore::Utils::Shader* m_TerrainShader;
    GLCore::Utils::Shader* m_MeshShader;
};

inline Renderer* g_Renderer = nullptr;

}

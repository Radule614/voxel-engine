#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>

#include "Vertex.hpp"
#include "Chunk.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace Terrain
{
VoxelLayer::VoxelLayer() : m_CameraController(45.0f, 16.0f / 9.0f)
{
    //TEMP
    m_TextureManager = TextureManager();
}

VoxelLayer::~VoxelLayer()
{
}

void VoxelLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_Shader = Shader::FromGLSLTextFiles("VoxelEngine/assets/shaders/default.vert.glsl",
                                         "VoxelEngine/assets/shaders/default.frag.glsl");
    m_TextureAtlas = m_TextureManager.LoadTexture("VoxelEngine/assets/textures/atlas.png", "texture_diffuse");

    glCreateVertexArrays(1, &m_VA);
    glBindVertexArray(m_VA);

    m_Chunk = Chunk(glm::vec3(0, -1, -1));

    std::vector<Vertex> vertices = m_Chunk.GetMesh();

    glCreateBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Vertex::Normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Vertex::Texture)));

    m_Indices = {};
    size_t faceCount = vertices.size() / 4;
    for (size_t i = 0; i < faceCount; ++i)
    {
        m_Indices.push_back(i * 4 + 0);
        m_Indices.push_back(i * 4 + 1);
        m_Indices.push_back(i * 4 + 2);
        m_Indices.push_back(i * 4 + 2);
        m_Indices.push_back(i * 4 + 3);
        m_Indices.push_back(i * 4 + 0);
    }

    glCreateBuffers(1, &m_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void VoxelLayer::OnDetach()
{
    glDeleteVertexArrays(1, &m_VA);
    glDeleteBuffers(1, &m_VB);
    glDeleteBuffers(1, &m_IB);
}


void VoxelLayer::OnEvent(GLCore::Event &event)
{
    m_CameraController.OnEvent(event);
}

void VoxelLayer::OnUpdate(Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_Shader->GetRendererID());

    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));
    location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Model");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_Chunk.GetModelMatrix()));

    glActiveTexture(GL_TEXTURE0);
    location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Atlas");
    glUniform1i(location, 0);
    glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

    glBindVertexArray(m_VA);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void VoxelLayer::OnImGuiRender()
{
}
}; // namespace Terrain

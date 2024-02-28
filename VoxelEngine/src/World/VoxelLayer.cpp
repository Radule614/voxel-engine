#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>

using namespace GLCore;
using namespace GLCore::Utils;

VoxelLayer::VoxelLayer() : m_CameraController(16.0f / 9.0f)
{
}

VoxelLayer::~VoxelLayer()
{
}

void VoxelLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Shader =
        Shader::FromGLSLTextFiles("VoxelEngine/assets/default.vert.glsl", "VoxelEngine/assets/default.frag.glsl");

    glCreateVertexArrays(1, &m_VA);
    glBindVertexArray(m_VA);

    Voxel voxel;
    voxel.SetAllFacesVisible(true);

    VoxelMeshBuilder meshBuilder;
    std::vector<float_t> vertices = meshBuilder.FromVoxel(voxel);

    glCreateBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float_t), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 8, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 8, (void *)(sizeof(float_t) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 8, (void *)(sizeof(float_t) * 6));

    m_Indices = {};
    size_t faceCount = vertices.size() / 8 / 4;
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

    // per instance data
    std::vector<float_t> instanceData = {2.0f, -2.0f, -1.0f};
    glGenBuffers(1, &m_InstanceVB);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * 3, &instanceData[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 3, 0);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

void VoxelLayer::OnDetach()
{
    glDeleteVertexArrays(1, &m_VA);
    glDeleteBuffers(1, &m_VB);
    glDeleteBuffers(1, &m_IB);
    glDeleteBuffers(1, &m_InstanceVB);
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
    location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
    glUniform4fv(location, 1, glm::value_ptr(m_Color));

    glBindVertexArray(m_VA);
    glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, 1);
    glBindVertexArray(0);
}

void VoxelLayer::OnImGuiRender()
{
}

#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Normal;
layout (location = 2) in vec2 i_TexCoords;
layout (location = 3) in vec4 i_Tangent;
layout (location = 4) in vec4 i_Joints;
layout (location = 5) in vec4 i_Weights;

out o_Vertex
{
    vec3 FragNormal;
    vec2 FragTexCoords;
    vec3 FragPosition;
    mat3 TBN;
    vec4 FragLightSpacePosition;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

uniform mat4 u_JointMatrices[128];
uniform bool u_IsSkinned;

#include "common.vert.glsl"

void main()
{
    VertexData data = CalculateWorldVertexData();

    vec3 N = normalize(data.Normal);
    vec3 T = normalize(data.Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * i_Tangent.w;

    o_VertexOut.FragNormal = N;
    o_VertexOut.FragPosition = vec3(data.Position);
    o_VertexOut.FragTexCoords = i_TexCoords;
    o_VertexOut.TBN = mat3(T, B, N);
    o_VertexOut.FragLightSpacePosition = u_LightSpaceMatrix * data.Position;

    gl_Position = u_ViewProjection * data.Position;
}
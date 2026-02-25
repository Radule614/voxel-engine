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

mat4 CalculateSkinMatrix()
{
    mat4 matrix = mat4(0.0);

    matrix += u_JointMatrices[int(i_Joints.x)] * i_Weights.x;
    matrix += u_JointMatrices[int(i_Joints.y)] * i_Weights.y;
    matrix += u_JointMatrices[int(i_Joints.z)] * i_Weights.z;
    matrix += u_JointMatrices[int(i_Joints.w)] * i_Weights.w;

    return matrix;
}

void main()
{
    vec4 position;
    vec3 normal;
    vec3 tangent;

    if (u_IsSkinned)
    {
        mat4 skin = CalculateSkinMatrix();

        position = skin * vec4(i_Position, 1.0);
        normal = mat3(skin) * i_Normal;
        tangent = mat3(skin) * i_Tangent.xyz;
    }
    else
    {
        position = u_Model * vec4(i_Position, 1.0);
        normal = mat3(u_Model) * i_Normal;
        tangent = mat3(u_Model) * i_Tangent.xyz;
    }

    vec3 N = normalize(normal);
    vec3 T = normalize(tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * i_Tangent.w;

    o_VertexOut.FragNormal = N;
    o_VertexOut.FragPosition = vec3(position);
    o_VertexOut.FragTexCoords = i_TexCoords;
    o_VertexOut.TBN = mat3(T, B, N);
    o_VertexOut.FragLightSpacePosition = u_LightSpaceMatrix * position;

    gl_Position = u_ViewProjection * position;
}
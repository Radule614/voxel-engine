#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Normal;
layout (location = 2) in vec2 i_TexCoords;
layout (location = 3) in vec4 i_Tangent;

out o_Vertex
{
    vec3 FragNormal;
    vec2 FragTexCoords;
    vec3 FragPosition;
    mat3 TBN;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    vec4 position = u_Model * vec4(i_Position, 1.0f);
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    vec3 normal = normalMatrix * i_Normal;

    vec3 N = normalize(normal);
    vec3 T = normalize(normalMatrix * i_Tangent.xyz);
    vec3 B = normalize(cross(N, T)) * i_Tangent.w;

    o_VertexOut.FragNormal = normal;
    o_VertexOut.FragPosition = vec3(position);
    o_VertexOut.FragTexCoords = i_TexCoords;
    o_VertexOut.TBN = mat3(T, B, N);

    gl_Position = u_ViewProjection * position;
}
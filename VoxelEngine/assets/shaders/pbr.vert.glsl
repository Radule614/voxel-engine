#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Normal;
layout (location = 2) in vec2 i_TexCoords;

out o_Vertex {
    vec3 FragNormal;
    vec3 FragPosition;
    vec2 FragTexCoords;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main() {
    vec4 position = u_Model * vec4(i_Position, 1.0f);
    vec3 normal = mat3(transpose(inverse(u_Model))) * i_Normal;

    o_VertexOut.FragNormal = normal;
    o_VertexOut.FragTexCoords = i_TexCoords;
    o_VertexOut.FragPosition = vec3(position);

    gl_Position = u_ViewProjection * position;
}
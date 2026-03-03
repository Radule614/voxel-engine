#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 2) in vec2 i_TexCoords;

out o_VertexOut
{
    vec2 FragTexCoords;
} o_Vertex;

void main()
{
    o_Vertex.FragTexCoords = i_TexCoords;
    gl_Position = vec4(i_Position, 1.0);
}
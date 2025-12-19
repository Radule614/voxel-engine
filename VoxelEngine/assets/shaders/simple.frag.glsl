#version 450 core

layout (location = 0) out vec4 o_Color;

in o_Vertex
{
    vec3 FragNormal;
    vec3 FragPosition;
    vec2 FragTexCoords;
} i_Fragment;

uniform vec3 u_Color;

void main()
{
    o_Color = vec4(u_Color, 1.0);
}

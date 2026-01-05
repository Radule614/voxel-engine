#version 450 core

layout (location = 0) in vec3 i_Position;

out o_Vertex
{
    vec3 LocalPosition;
} o_VertexOut;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    o_VertexOut.LocalPosition = i_Position;

    gl_Position = u_Projection * u_View * vec4(o_VertexOut.LocalPosition, 1.0);
}
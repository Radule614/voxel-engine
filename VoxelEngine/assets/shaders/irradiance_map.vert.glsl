#version 450 core

layout (location = 0) in vec3 i_Position;

out VertexOut
{
    vec3 LocalPosition;
} o_Vertex;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    o_Vertex.LocalPosition = i_Position;

    gl_Position = u_Projection * u_View * vec4(i_Position, 1.0);
}